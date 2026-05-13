package ru.carshow.dto;

import java.math.BigDecimal;
import java.util.List;
import java.util.UUID;

public class Part {
    private UUID id;
    private String name;
    private String category; 
    private BigDecimal price;
    private String manufacturer;
    private String partNumber;
    private Integer quantity;
    private List<UUID> compatibleCarModelIds;
    
    public Part() {}
    
    public UUID getId() { return id; }
    public void setId(UUID id) { this.id = id; }
    public String getName() { return name; }
    public void setName(String name) { this.name = name; }
    public String getCategory() { return category; }
    public void setCategory(String category) { this.category = category; }
    public BigDecimal getPrice() { return price; }
    public void setPrice(BigDecimal price) { this.price = price; }
    public String getManufacturer() { return manufacturer; }
    public void setManufacturer(String manufacturer) { this.manufacturer = manufacturer; }
    public String getPartNumber() { return partNumber; }
    public void setPartNumber(String partNumber) { this.partNumber = partNumber; }
    public Integer getQuantity() { return quantity; }
    public void setQuantity(Integer quantity) { this.quantity = quantity; }
    public List<UUID> getCompatibleCarModelIds() { return compatibleCarModelIds; }
    public void setCompatibleCarModelIds(List<UUID> compatibleCarModelIds) { this.compatibleCarModelIds = compatibleCarModelIds; }
}