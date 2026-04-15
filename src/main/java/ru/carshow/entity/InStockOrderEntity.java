package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@DiscriminatorValue("InStockOrder")
public class InStockOrderEntity extends OrderEntity {
    
    @ManyToOne @JoinColumn(name = "car_id")
    private CarEntity car;
    
    public InStockOrderEntity() {}
}