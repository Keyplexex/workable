package ru.carshow.dto;

import java.math.BigDecimal;

public class CarFilterRequest {
    private String brand;
    private String model;
    private BigDecimal minPrice;
    private BigDecimal maxPrice;
    private String fuelType;
    private String transmission;
    private String driveType;
    private String bodyType;
    private BigDecimal minEnginePower;
    private BigDecimal maxEnginePower;
    
    public CarFilterRequest() {}
    
    public String getBrand() { return brand; }
    public void setBrand(String brand) { this.brand = brand; }
    public String getModel() { return model; }
    public void setModel(String model) { this.model = model; }
    public BigDecimal getMinPrice() { return minPrice; }
    public void setMinPrice(BigDecimal minPrice) { this.minPrice = minPrice; }
    public BigDecimal getMaxPrice() { return maxPrice; }
    public void setMaxPrice(BigDecimal maxPrice) { this.maxPrice = maxPrice; }
    public String getFuelType() { return fuelType; }
    public void setFuelType(String fuelType) { this.fuelType = fuelType; }
    public String getTransmission() { return transmission; }
    public void setTransmission(String transmission) { this.transmission = transmission; }
    public String getDriveType() { return driveType; }
    public void setDriveType(String driveType) { this.driveType = driveType; }
    public String getBodyType() { return bodyType; }
    public void setBodyType(String bodyType) { this.bodyType = bodyType; }
    public BigDecimal getMinEnginePower() { return minEnginePower; }
    public void setMinEnginePower(BigDecimal minEnginePower) { this.minEnginePower = minEnginePower; }
    public BigDecimal getMaxEnginePower() { return maxEnginePower; }
    public void setMaxEnginePower(BigDecimal maxEnginePower) { this.maxEnginePower = maxEnginePower; }
}