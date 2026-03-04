package ru.carshow.repository;

import ru.carshow.domain.model.order.Order;
import ru.carshow.domain.model.order.OrderStatus;
import ru.carshow.domain.model.user.Client;
import ru.carshow.domain.model.user.Manager;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public class OrderRepository {
    private final Map<Long, Order> orders = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public Order save(Order order) {
        if (order.getId() == null) {
            long newId = generateId();
            try {
                Order newOrder = order.getClass()
                        .getDeclaredConstructor(Long.class, Client.class, Manager.class)
                        .newInstance(newId, order.getClient(), order.getManager());
                
                if (order.getStatus() != OrderStatus.CREATED) {
                    newOrder.setStatus(order.getStatus());
                }
                
                orders.put(newId, newOrder);
                return newOrder;
            } catch (Exception e) {
                throw new RuntimeException("Не удалось создать копию заказа с новым ID", e);
            }
        } else {
            orders.put(order.getId(), order);
            return order;
        }
    }
    
    public Optional<Order> findById(Long id) {
        return Optional.ofNullable(orders.get(id));
    }
    
    public Order getById(Long id) {
        return findById(id)
            .orElseThrow(() -> new EntityNotFoundException("Заказ с ID " + id + " не найден"));
    }
    
    public List<Order> findAll() {
        return new ArrayList<>(orders.values());
    }
    
    public List<Order> findByStatus(OrderStatus status) {
        return orders.values().stream()
            .filter(order -> order.getStatus() == status)
            .collect(Collectors.toList());
    }
    
    public List<Order> findByClientId(Long clientId) {
        return orders.values().stream()
            .filter(order -> order.getClient().getId().equals(clientId))
            .collect(Collectors.toList());
    }
    
    public List<Order> findByManagerId(Long managerId) {
        return orders.values().stream()
            .filter(order -> order.getManager() != null && 
                   order.getManager().getId().equals(managerId))
            .collect(Collectors.toList());
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
}