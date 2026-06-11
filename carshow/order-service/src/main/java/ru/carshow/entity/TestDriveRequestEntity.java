package ru.carshow.entity;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.EnumType;
import jakarta.persistence.Enumerated;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.ManyToOne;
import jakarta.persistence.Table;
import lombok.Getter;
import lombok.Setter;

import java.time.LocalDateTime;
import java.util.UUID;

@Getter
@Setter
@Entity
@Table(name = "test_drive_requests")
public class TestDriveRequestEntity extends BaseEntity {

    @ManyToOne
    @JoinColumn(name = "client_id")
    private ClientEntity client;

    @Column(name = "car_id", nullable = false)
    private UUID carId;

    private LocalDateTime startDateTime;

    @Enumerated(EnumType.STRING)
    private TestDriveStatus status;

    public TestDriveRequestEntity() {}
}
