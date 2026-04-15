package ru.carshow.dto;

import java.math.BigDecimal;

public class UpdateCarRequest {
    private String color;
    private BigDecimal price;
    private String status;
    
    public UpdateCarRequest() {}
    
    public String getColor() { return color; }
    public void setColor(String color) { this.color = color; }
    public BigDecimal getPrice() { return price; }
    public void setPrice(BigDecimal price) { this.price = price; }
    public String getStatus() { return status; }
    public void setStatus(String status) { this.status = status; }
}