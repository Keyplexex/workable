package ru.carshow.entity;

import jakarta.persistence.Entity;
import jakarta.persistence.Table;
import lombok.Getter;
import lombok.Setter;

import java.math.BigDecimal;

@Getter
@Setter
@Entity
@Table(name = "car_models")
public class CarModelEntity extends BaseEntity {

    private String brand;
    private String name;
    private BigDecimal basePrice;
    private String bodyType;
}
