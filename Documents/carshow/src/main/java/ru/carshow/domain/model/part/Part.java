package ru.carshow.domain.model.part;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

public class Part {
    private final Long id;
    private final String name;
    private final PartCategory category;
    private BigDecimal price;
    private String manufacturer;
    private String partNumber;
    private int quantity;
    private Set<Long> compatibleCarModelIds;
    
    public Part(Long id, String name, PartCategory category, BigDecimal price, 
                String manufacturer, String partNumber, int quantity) {
        this.id = id;
        this.name = name;
        this.category = category;
        this.price = price;
        this.manufacturer = manufacturer;
        this.partNumber = partNumber;
        this.quantity = quantity;
        this.compatibleCarModelIds = new HashSet<>();
    }
    
    public Long getId() { return id; }
    public String getName() { return name; }
    public PartCategory getCategory() { return category; }
    public BigDecimal getPrice() { return price; }
    public String getManufacturer() { return manufacturer; }
    public String getPartNumber() { return partNumber; }
    public int getQuantity() { return quantity; }
    public Set<Long> getCompatibleCarModelIds() { return compatibleCarModelIds; }
    
    public void setPrice(BigDecimal price) { this.price = price; }
    public void setManufacturer(String manufacturer) { this.manufacturer = manufacturer; }
    public void setPartNumber(String partNumber) { this.partNumber = partNumber; }
    public void setQuantity(int quantity) { this.quantity = quantity; }
    
    public void addCompatibleCarModel(Long carModelId) {
        compatibleCarModelIds.add(carModelId);
    }
    
    public void removeCompatibleCarModel(Long carModelId) {
        compatibleCarModelIds.remove(carModelId);
    }
    
    public boolean isCompatibleWithCarModel(Long carModelId) {
        return compatibleCarModelIds.contains(carModelId);
    }
    
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Part part = (Part) o;
        return Objects.equals(id, part.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}