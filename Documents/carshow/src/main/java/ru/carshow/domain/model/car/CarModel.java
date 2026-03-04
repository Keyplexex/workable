package ru.carshow.domain.model.car;

import java.math.BigDecimal;
import java.util.Objects;

public class CarModel {
    private final Long id;
    private final CarBrand brand;
    private final String name;
    private final BodyType bodyType;
    private final BigDecimal basePrice;

    public CarModel(Long id, CarBrand brand, String name, BodyType bodyType, BigDecimal basePrice) {
        this.id = id;
        this.brand = brand;
        this.name = name;
        this.bodyType = bodyType;
        this.basePrice = basePrice;
    }

    public Long getId() {
        return id;
    }
    public CarBrand getBrand() {
        return brand;
    }
    public String getName() {
        return name;
    }
    public BodyType getBodyType() {
        return bodyType;
    }
    public BigDecimal getBasePrice() {
        return basePrice;
    }

    @Override
    public boolean equals(Object c) {
        if (this == c) {
            return true;
        }
        if (c == null || getClass() != c.getClass()) {
            return false;
        }
        CarModel carModel = (CarModel) c;
        return Objects.equals(id, carModel.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}
