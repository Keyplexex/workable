package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.time.LocalDateTime;
import java.util.UUID;

@Getter @Setter @Entity @Table(name = "test_drive_requests")
public class TestDriveRequestEntity extends BaseEntity {
    
    @ManyToOne @JoinColumn(name = "client_id")
    private ClientEntity client;
    
    @ManyToOne @JoinColumn(name = "car_id")
    private CarEntity car;
    
    private LocalDateTime startDateTime;
    
    @Enumerated(EnumType.STRING)
    private TestDriveStatus status; 
    
    public TestDriveRequestEntity() {}
}