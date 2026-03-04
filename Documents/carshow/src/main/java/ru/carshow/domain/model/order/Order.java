package ru.carshow.domain.model.order;

import ru.carshow.domain.model.user.Client;
import ru.carshow.domain.model.user.Manager;

import java.time.LocalDateTime;
import java.util.Objects;

public abstract class Order {
    private final Long id;
    private final Client client;
    private Manager manager;
    private OrderStatus status;
    private final LocalDateTime createdAt;
    private LocalDateTime updatedAt;
    
    public Order(Long id, Client client, Manager manager) {
        this.id = id;
        this.client = client;
        this.manager = manager;
        this.status = OrderStatus.CREATED;
        this.createdAt = LocalDateTime.now();
        this.updatedAt = LocalDateTime.now();
    }
    
    public Long getId() { 
        return id; 
    }
    public Client getClient() { 
        return client; 
    }
    public Manager getManager() { 
        return manager; 
    }
    public OrderStatus getStatus() { 
        return status; 
    }
    public LocalDateTime getCreatedAt() { 
        return createdAt; 
    }
    public LocalDateTime getUpdatedAt() { 
        return updatedAt; 
    }
    
    public void setManager(Manager manager) { 
        this.manager = manager; 
        this.updatedAt = LocalDateTime.now();
    }
    
    public void setStatus(OrderStatus status) { 
        this.status = status; 
        this.updatedAt = LocalDateTime.now();
    }
    
    public abstract String getOrderType();
    
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Order order = (Order) o;
        return Objects.equals(id, order.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}