package ru.carshow.entity;

import jakarta.persistence.Column;
import jakarta.persistence.DiscriminatorValue;
import jakarta.persistence.Entity;
import lombok.Getter;
import lombok.Setter;

import java.util.UUID;

@Getter
@Setter
@Entity
@DiscriminatorValue("InStockOrder")
public class InStockOrderEntity extends OrderEntity {

    @Column(name = "car_id")
    private UUID carId;

    public InStockOrderEntity() {}
}
