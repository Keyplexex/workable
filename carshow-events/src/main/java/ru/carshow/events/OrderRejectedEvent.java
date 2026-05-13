package ru.carshow.events;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.UUID;

public record OrderRejectedEvent(
        @JsonProperty("eventId") UUID eventId,
        @JsonProperty("traceId") UUID traceId,
        @JsonProperty("orderId") UUID orderId,
        @JsonProperty("reason") String reason,
        @JsonProperty("assemblyOrderId") UUID assemblyOrderId
) {}
