package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@Table(name = "orders")
@Inheritance(strategy = InheritanceType.SINGLE_TABLE)
@DiscriminatorColumn(name = "dtype", discriminatorType = DiscriminatorType.STRING)  
public abstract class OrderEntity extends BaseEntity {
    
    @ManyToOne @JoinColumn(name = "client_id")
    private ClientEntity client;
    
    @ManyToOne @JoinColumn(name = "manager_id")
    private ManagerEntity manager;
    
    @Enumerated(EnumType.STRING)
    private OrderStatus status;
    
    public OrderEntity() {}
}