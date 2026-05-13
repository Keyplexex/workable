package ru.carshow.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import org.slf4j.MDC;
import org.springframework.security.access.AccessDeniedException;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.client.StorageInternalClient;
import ru.carshow.dto.*;
import ru.carshow.entity.*;
import ru.carshow.events.OrderSentForApprovalEvent;
import ru.carshow.events.RabbitNames;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.mapper.OrderMapper;
import ru.carshow.repository.*;
import ru.carshow.security.CurrentUser;
import ru.carshow.dto.request.BuildConfigurationRequest;

import java.math.BigDecimal;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Service
@RequiredArgsConstructor
@Transactional(readOnly = true)
public class OrderService {

    private final OrderRepository orderRepository;
    private final UserRepository userRepository;
    private final CarModelRepository carModelRepository;
    private final OrderMapper orderMapper;
    private final CurrentUser currentUser;
    private final ConfigurationService configurationService;
    private final OutboxMessageRepository outboxMessageRepository;
    private final ObjectMapper objectMapper;
    private final StorageInternalClient storageInternalClient;

    @Transactional
    public InStockOrder createInStockOrder(UUID carId) {
        UUID currentClientId = currentUser.getId();
        ClientEntity client = (ClientEntity) userRepository.findById(currentClientId)
                .filter(e -> !e.isRemoved())
                .orElseThrow(() -> new DomainValidationException("Client not found: " + currentClientId));

        if (!storageInternalClient.isCarAvailableForClientOperations(carId)) {
            throw new DomainValidationException("Car not available for order: " + carId);
        }

        ManagerEntity manager = findRandomManager();

        InStockOrderEntity order = new InStockOrderEntity();
        order.setClient(client);
        order.setManager(manager);
        order.setCarId(carId);
        order.setStatus(OrderStatus.DRAFT);

        return orderMapper.toResponse(orderRepository.save(order));
    }

    @Transactional
    public ConfigurationOrder createConfigurationOrder(UUID carModelId, Map<String, UUID> selectedComponents) {
        UUID currentClientId = currentUser.getId();
        ClientEntity client = (ClientEntity) userRepository.findById(currentClientId)
                .filter(e -> !e.isRemoved())
                .orElseThrow(() -> new DomainValidationException("Client not found: " + currentClientId));

        CarModelEntity carModel = carModelRepository.findById(carModelId)
                .filter(e -> !e.isRemoved())
                .orElseThrow(() -> new DomainValidationException("CarModel not found: " + carModelId));

        ManagerEntity manager = findRandomManager();

        BuildConfigurationRequest buildRequest = new BuildConfigurationRequest();
        buildRequest.setCarModelId(carModelId);
        buildRequest.setSelectedComponents(selectedComponents);
        var built = configurationService.buildConfiguration(buildRequest);

        ConfigurationOrderEntity order = new ConfigurationOrderEntity();
        order.setClient(client);
        order.setManager(manager);
        order.setCarModel(carModel);
        order.setStatus(OrderStatus.DRAFT);
        order.setSelectedComponents(selectedComponents);
        order.setFinalPrice(built.getFinalPrice());

        return orderMapper.toResponse(orderRepository.save(order));
    }

    public Order getOrderById(UUID id) {
        OrderEntity entity = orderRepository.findById(id)
                .filter(e -> !e.isRemoved())
                .orElseThrow(() -> new DomainValidationException("Order not found: " + id));

        if (!currentUser.hasRole("ADMIN") && !currentUser.hasRole("MANAGER")) {
            if (!entity.getClient().getId().equals(currentUser.getId())) {
                throw new AccessDeniedException("You can only view your own orders");
            }
        }
        return orderMapper.toResponse(entity);
    }

    public List<Order> getAllOrders() {
        if (currentUser.hasRole("ADMIN") || currentUser.hasRole("MANAGER")) {
            return StreamSupport.stream(orderRepository.findAll().spliterator(), false)
                    .filter(e -> !e.isRemoved())
                    .map(orderMapper::toResponse)
                    .collect(Collectors.toList());
        }
        return getOrdersByClient(currentUser.getId());
    }

    public List<Order> getOrdersByClient(UUID clientId) {
        return StreamSupport.stream(orderRepository.findByClientId(clientId).spliterator(), false)
                .filter(e -> !e.isRemoved())
                .map(orderMapper::toResponse)
                .collect(Collectors.toList());
    }

    public List<Order> getOrdersByManager(UUID managerId) {
        return StreamSupport.stream(orderRepository.findByManagerId(managerId).spliterator(), false)
                .filter(e -> !e.isRemoved())
                .map(orderMapper::toResponse)
                .collect(Collectors.toList());
    }

    @Transactional
    public Order updateOrderStatus(UUID orderId, OrderStatus newStatus) {
        OrderEntity entity = orderRepository.findById(orderId)
                .orElseThrow(() -> new DomainValidationException("Order not found: " + orderId));
        OrderStatus previous = entity.getStatus();
        entity.setStatus(newStatus);
        orderRepository.save(entity);

        if (newStatus == OrderStatus.PAID && previous != OrderStatus.PAID) {
            enqueuePaidOutbox(entity);
        }
        return orderMapper.toResponse(entity);
    }

    private void enqueuePaidOutbox(OrderEntity entity) {
        UUID traceId = UUID.randomUUID();
        UUID eventId = UUID.randomUUID();
        MDC.put("traceId", traceId.toString());
        MDC.put("orderId", entity.getId().toString());
        try {
            String orderType;
            UUID carId = null;
            UUID carModelId = null;
            List<UUID> componentIds = new ArrayList<>();

            if (entity instanceof InStockOrderEntity inStock) {
                orderType = "INSTOCK";
                carId = inStock.getCarId();
            } else if (entity instanceof ConfigurationOrderEntity cfg) {
                orderType = "CONFIGURATION";
                carModelId = cfg.getCarModel() != null ? cfg.getCarModel().getId() : null;
                componentIds.addAll(cfg.getSelectedComponents().values());
            } else {
                throw new DomainValidationException("Unsupported order type for warehouse flow");
            }

            OrderSentForApprovalEvent payload = new OrderSentForApprovalEvent(
                    eventId, traceId, entity.getId(), orderType, carId, carModelId, componentIds);

            OutboxMessageEntity row = new OutboxMessageEntity();
            row.setId(eventId);
            row.setCreatedAt(Instant.now());
            row.setAggregateType("ORDER");
            row.setAggregateId(entity.getId());
            row.setRoutingKey(RabbitNames.RK_ORDER_SENT);
            row.setPayload(writeJson(payload));

            outboxMessageRepository.save(row);
        } finally {
            MDC.remove("traceId");
            MDC.remove("orderId");
        }
    }

    private String writeJson(Object o) {
        try {
            return objectMapper.writeValueAsString(o);
        } catch (Exception e) {
            throw new IllegalStateException(e);
        }
    }

    @Transactional
    public void assignManagerToOrder(UUID orderId, UUID managerId) {
        OrderEntity order = orderRepository.findById(orderId)
                .orElseThrow(() -> new DomainValidationException("Order not found: " + orderId));
        ManagerEntity manager = (ManagerEntity) userRepository.findById(managerId)
                .orElseThrow(() -> new DomainValidationException("Manager not found: " + managerId));
        order.setManager(manager);
        orderRepository.save(order);
    }

    @Transactional
    public void cancelOrder(UUID orderId) {
        OrderEntity order = orderRepository.findById(orderId)
                .orElseThrow(() -> new DomainValidationException("Order not found: " + orderId));

        if (!currentUser.hasRole("ADMIN") && !order.getClient().getId().equals(currentUser.getId())) {
            throw new AccessDeniedException("You can only cancel your own orders");
        }

        order.setStatus(OrderStatus.CANCELLED);
        orderRepository.save(order);
    }

    private ManagerEntity findRandomManager() {
        return StreamSupport.stream(userRepository.findAll().spliterator(), false)
                .filter(u -> u instanceof ManagerEntity && !u.isRemoved())
                .map(u -> (ManagerEntity) u)
                .findFirst()
                .orElseThrow(() -> new DomainValidationException("No managers available"));
    }
}
