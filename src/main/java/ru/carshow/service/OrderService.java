package ru.carshow.service;

import lombok.RequiredArgsConstructor;
import org.springframework.security.access.AccessDeniedException;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.dto.*;
import ru.carshow.entity.*;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.mapper.OrderMapper;
import ru.carshow.repository.*;
import ru.carshow.security.CurrentUser;

import java.math.BigDecimal;
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
    private final CarRepository carRepository;
    private final CarModelRepository carModelRepository;
    private final OrderMapper orderMapper;
    private final CurrentUser currentUser;
    
    @Transactional
    public InStockOrder createInStockOrder(UUID carId) {
        UUID currentClientId = currentUser.getId();
        ClientEntity client = (ClientEntity) userRepository.findById(currentClientId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Client not found: " + currentClientId));
        
        CarEntity car = carRepository.findById(carId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Car not found: " + carId));
        
        ManagerEntity manager = findRandomManager();
        
        InStockOrderEntity order = new InStockOrderEntity();
        order.setClient(client);
        order.setManager(manager);
        order.setCar(car);
        order.setStatus(OrderStatus.DRAFT);
        
        return orderMapper.toResponse(orderRepository.save(order));
    }
    
    @Transactional
    public ConfigurationOrder createConfigurationOrder(UUID carModelId, 
                                                       Map<String, UUID> selectedComponents) {
        UUID currentClientId = currentUser.getId();
        ClientEntity client = (ClientEntity) userRepository.findById(currentClientId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Client not found: " + currentClientId));
        
        CarModelEntity carModel = carModelRepository.findById(carModelId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("CarModel not found: " + carModelId));
        
        ManagerEntity manager = findRandomManager();
        
        ConfigurationOrderEntity order = new ConfigurationOrderEntity();
        order.setClient(client);
        order.setManager(manager);
        order.setCarModel(carModel);
        order.setStatus(OrderStatus.DRAFT);
        order.setSelectedComponentsJson("{}"); 
        
        order.setFinalPrice(BigDecimal.valueOf(carModel.getBasePrice().doubleValue()));
        
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
        } else {
            return getOrdersByClient(currentUser.getId());
        }
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
        entity.setStatus(newStatus);
        return orderMapper.toResponse(orderRepository.save(entity));
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