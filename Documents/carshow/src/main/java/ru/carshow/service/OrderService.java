package ru.carshow.service;

import ru.carshow.domain.model.car.Car;
import ru.carshow.domain.model.car.CarModel;
import ru.carshow.domain.model.order.*;
import ru.carshow.domain.model.user.Client;
import ru.carshow.domain.model.user.Manager;
import ru.carshow.repository.OrderRepository;
import ru.carshow.repository.UserRepository;
import ru.carshow.repository.CarRepository;
import ru.carshow.domain.exception.DomainValidationException;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.util.List;
import java.util.Random;

public class OrderService {
    private final OrderRepository orderRepository;
    private final UserRepository userRepository;
    private final CarRepository carRepository;
    private final Random random = new Random();
    
    public OrderService(OrderRepository orderRepository, UserRepository userRepository, 
                       CarRepository carRepository) {
        this.orderRepository = orderRepository;
        this.userRepository = userRepository;
        this.carRepository = carRepository;
    }
    
    public InStockOrder createInStockOrder(Long clientId, Long carId) {
        Client client = getClientById(clientId);
        Car car = carRepository.getById(carId);
        Manager manager = assignRandomManager();
        
        Long id = orderRepository.generateId();
        InStockOrder order = new InStockOrder(id, client, manager, car);
        
        return (InStockOrder) orderRepository.save(order);
    }
    
    public ConfigurationOrder createConfigurationOrder(Long clientId, Long carModelId) {
        Client client = getClientById(clientId);
        CarModel carModel = getCarModelById(carModelId);
        Manager manager = assignRandomManager();
        
        Long id = orderRepository.generateId();
        ConfigurationOrder order = new ConfigurationOrder(id, client, manager, carModel);
        
        return (ConfigurationOrder) orderRepository.save(order);
    }
    
    public Order getOrderById(Long id) {
        return orderRepository.getById(id);
    }
    
    public List<Order> getAllOrders() {
        return orderRepository.findAll();
    }
    
    public List<Order> getOrdersByClient(Long clientId) {
        return orderRepository.findByClientId(clientId);
    }
    
    public List<Order> getOrdersByManager(Long managerId) {
        return orderRepository.findByManagerId(managerId);
    }
    
    public Order updateOrderStatus(Long orderId, OrderStatus newStatus) {
        Order order = orderRepository.getById(orderId);
        validateStatusTransition(order.getStatus(), newStatus);
        
        order.setStatus(newStatus);
        return orderRepository.save(order);
    }
    
    public void assignManagerToOrder(Long orderId, Long managerId) {
        Order order = orderRepository.getById(orderId);
        Manager manager = (Manager) userRepository.getById(managerId);
        
        if (!"MANAGER".equals(manager.getRole())) {
            throw new DomainValidationException("Пользователь не является менеджером");
        }
        
        order.setManager(manager);
        orderRepository.save(order);
    }
    
    public void cancelOrder(Long orderId) {
        Order order = orderRepository.getById(orderId);
        if (order.getStatus() == OrderStatus.COMPLETED) {
            throw new DomainValidationException("Нельзя отменить выполненный заказ");
        }
        order.setStatus(OrderStatus.CANCELLED);
        orderRepository.save(order);
    }
    
    private Client getClientById(Long clientId) {
        return (Client) userRepository.getById(clientId);
    }
    
    private CarModel getCarModelById(Long carModelId) {

        throw new UnsupportedOperationException("Метод временно не реализован");
    }
    
    private Manager assignRandomManager() {
        List<Manager> managers = userRepository.findAll().stream()
            .filter(user -> "MANAGER".equals(user.getRole()))
            .map(user -> (Manager) user)
            .toList();
        
        if (managers.isEmpty()) {
            throw new DomainValidationException("Нет доступных менеджеров");
        }
        
        return managers.get(random.nextInt(managers.size()));
    }
    
    private void validateStatusTransition(OrderStatus current, OrderStatus next) {
        if (current == OrderStatus.CANCELLED || current == OrderStatus.COMPLETED) {
            throw new DomainValidationException("Нельзя изменить статус завершенного или отмененного заказа");
        }
    }
}