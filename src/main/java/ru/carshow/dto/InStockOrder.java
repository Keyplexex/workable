package ru.carshow.dto;

import java.util.UUID;

public class InStockOrder extends Order {
    private UUID carId;
    
    public InStockOrder() { super(); }
    
    public UUID getCarId() { return carId; }
    public void setCarId(UUID carId) { this.carId = carId; }
}