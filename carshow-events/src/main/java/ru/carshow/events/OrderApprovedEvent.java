package ru.carshow.events;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.UUID;

public record OrderApprovedEvent(
        @JsonProperty("eventId") UUID eventId,
        @JsonProperty("traceId") UUID traceId,
        @JsonProperty("orderId") UUID orderId,
        @JsonProperty("assemblyOrderId") UUID assemblyOrderId
) {}
