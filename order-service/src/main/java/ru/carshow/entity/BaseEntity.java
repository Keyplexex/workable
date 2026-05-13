package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.time.Instant;
import java.util.UUID;

@Getter @Setter @MappedSuperclass
public abstract class BaseEntity {
    
    @Id @GeneratedValue(strategy = GenerationType.UUID)
    @Column(columnDefinition = "uuid")
    private UUID id;
    
    @Column(name = "created_at", nullable = false, updatable = false)
    private Instant createdAt;
    
    @Column(name = "updated_at")
    private Instant updatedAt;
    
    @Column(name = "removed", nullable = false)
    private boolean removed = false;
    
    @PrePersist protected void onCreate() {
        createdAt = Instant.now(); updatedAt = Instant.now();
        if (id == null) id = UUID.randomUUID();
    }
    
    @PreUpdate protected void onUpdate() { updatedAt = Instant.now(); }
}