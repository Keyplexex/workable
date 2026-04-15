package ru.carshow.dto;

import java.util.Map;
import java.util.UUID;

public class ConfigurationOrder extends Order {
    private UUID carModelId;
    private Map<String, UUID> selectedComponents; 
    private Double finalPrice;
    
    public ConfigurationOrder() { super(); }
    
    public UUID getCarModelId() { return carModelId; }
    public void setCarModelId(UUID carModelId) { this.carModelId = carModelId; }
    public Map<String, UUID> getSelectedComponents() { return selectedComponents; }
    public void setSelectedComponents(Map<String, UUID> selectedComponents) { this.selectedComponents = selectedComponents; }
    public Double getFinalPrice() { return finalPrice; }
    public void setFinalPrice(Double finalPrice) { this.finalPrice = finalPrice; }
}