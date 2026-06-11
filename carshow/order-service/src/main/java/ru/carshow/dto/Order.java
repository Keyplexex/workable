package ru.carshow.dto;

import java.time.Instant;
import java.util.UUID;

public class Order {
    private UUID id;
    private UUID clientId;
    private UUID managerId;
    private String status; 
    private Instant createdAt;
    
    public Order() {}
    
    public UUID getId() { return id; }
    public void setId(UUID id) { this.id = id; }
    public UUID getClientId() { return clientId; }
    public void setClientId(UUID clientId) { this.clientId = clientId; }
    public UUID getManagerId() { return managerId; }
    public void setManagerId(UUID managerId) { this.managerId = managerId; }
    public String getStatus() { return status; }
    public void setStatus(String status) { this.status = status; }
    public Instant getCreatedAt() { return createdAt; }
    public void setCreatedAt(Instant createdAt) { this.createdAt = createdAt; }
}