package ru.carshow.service;

import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.dto.*;
import ru.carshow.entity.*;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.mapper.OrderMapper;
import ru.carshow.repository.*;

import java.util.List;
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
    
    @Transactional
    public InStockOrder createInStockOrder(UUID clientId, UUID carId) {
        ClientEntity client = (ClientEntity) userRepository.findById(clientId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Client not found: " + clientId));
        
        CarEntity car = carRepository.findById(carId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Car not found: " + carId));
        
        ManagerEntity manager = findRandomManager();
        
        InStockOrderEntity order = new InStockOrderEntity();
        order.setClient(client);
        order.setManager(manager);
        order.setCar(car);
        order.setStatus(OrderStatus.DRAFT);
        
        InStockOrderEntity saved = orderRepository.save(order);
        return orderMapper.toResponse(saved);
    }
    
    @Transactional
    public ConfigurationOrder createConfigurationOrder(UUID clientId, UUID carModelId, 
                                                       java.util.Map<String, UUID> selectedComponents) {
        ClientEntity client = (ClientEntity) userRepository.findById(clientId)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Client not found: " + clientId));
        
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
        
        Double finalPrice = carModel.getBasePrice().doubleValue();
        order.setFinalPrice(java.math.BigDecimal.valueOf(finalPrice));
        
        ConfigurationOrderEntity saved = orderRepository.save(order);
        return orderMapper.toResponse(saved);
    }
    
    public Order getOrderById(UUID id) {
        OrderEntity entity = orderRepository.findById(id)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Order not found: " + id));
        return orderMapper.toResponse(entity);
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
        OrderEntity updated = orderRepository.save(entity);
        return orderMapper.toResponse(updated);
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