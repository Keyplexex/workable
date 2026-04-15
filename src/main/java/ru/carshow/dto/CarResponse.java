package ru.carshow.dto;

import java.math.BigDecimal;
import java.util.UUID;

public class CarResponse {
    private UUID id;
    private String vin;
    private Integer year;
    private String color;
    private BigDecimal enginePower;
    private BigDecimal engineVolume;  
    private String fuelType;           
    private String transmission;     
    private String driveType;        
    private BigDecimal price;
    private String status;          
    private UUID modelId;
    private String modelBrand;
    private String modelName;
    
    public CarResponse() {}
    
    public UUID getId() { return id; }
    public String getVin() { return vin; }
    public Integer getYear() { return year; }
    public String getColor() { return color; }
    public BigDecimal getEnginePower() { return enginePower; }
    public void setEnginePower(BigDecimal enginePower) { this.enginePower = enginePower; }
    public BigDecimal getEngineVolume() { return engineVolume; }
    public void setEngineVolume(BigDecimal engineVolume) { this.engineVolume = engineVolume; }
    public String getFuelType() { return fuelType; }
    public String getTransmission() { return transmission; }
    public String getDriveType() { return driveType; }
    public BigDecimal getPrice() { return price; }
    public String getStatus() { return status; }
    public UUID getModelId() { return modelId; }
    public String getModelBrand() { return modelBrand; }
    public String getModelName() { return modelName; }
    
    public void setId(UUID id) { this.id = id; }
    public void setVin(String vin) { this.vin = vin; }
    public void setYear(Integer year) { this.year = year; }
    public void setColor(String color) { this.color = color; }
    public void setFuelType(String fuelType) { this.fuelType = fuelType; }
    public void setTransmission(String transmission) { this.transmission = transmission; }
    public void setDriveType(String driveType) { this.driveType = driveType; }
    public void setPrice(BigDecimal price) { this.price = price; }
    public void setStatus(String status) { this.status = status; }
    public void setModelId(UUID modelId) { this.modelId = modelId; }
    public void setModelBrand(String modelBrand) { this.modelBrand = modelBrand; }
    public void setModelName(String modelName) { this.modelName = modelName; }
}