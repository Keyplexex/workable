package ru.carshow.events;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.List;
import java.util.UUID;

public record OrderSentForApprovalEvent(
        @JsonProperty("eventId") UUID eventId,
        @JsonProperty("traceId") UUID traceId,
        @JsonProperty("orderId") UUID orderId,
        @JsonProperty("orderType") String orderType,
        @JsonProperty("carId") UUID carId,
        @JsonProperty("carModelId") UUID carModelId,
        @JsonProperty("selectedComponentIds") List<UUID> selectedComponentIds
) {}
