package ru.carshow.entity;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import lombok.Getter;
import lombok.Setter;

import java.time.Instant;
import java.util.UUID;

@Getter
@Setter
@Entity
@Table(name = "outbox_message")
public class OutboxMessageEntity {

    @Id
    private UUID id;

    @Column(nullable = false)
    private Instant createdAt;

    private Instant publishedAt;

    @Column(nullable = false, length = 64)
    private String aggregateType;

    @Column(nullable = false)
    private UUID aggregateId;

    @Column(nullable = false, length = 128)
    private String routingKey;

    @Column(nullable = false, columnDefinition = "text")
    private String payload;
}
