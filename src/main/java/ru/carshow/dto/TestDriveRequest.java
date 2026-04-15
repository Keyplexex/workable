package ru.carshow.dto;

import java.time.LocalDateTime;
import java.util.UUID;

public class TestDriveRequest {
    private UUID id;
    private UUID clientId;
    private UUID carId;
    private LocalDateTime startDateTime;
    private String status; 
    
    public TestDriveRequest() {}
    
    public UUID getId() { return id; }
    public UUID getClientId() { return clientId; }
    public UUID getCarId() { return carId; }
    public LocalDateTime getStartDateTime() { return startDateTime; }
    public String getStatus() { return status; }
    
    public void setId(UUID id) { this.id = id; }
    public void setClientId(UUID clientId) { this.clientId = clientId; }
    public void setCarId(UUID carId) { this.carId = carId; }
    public void setStartDateTime(LocalDateTime startDateTime) { this.startDateTime = startDateTime; }
    public void setStatus(String status) { this.status = status; }
}