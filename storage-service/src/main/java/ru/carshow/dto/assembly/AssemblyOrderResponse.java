package ru.carshow.dto.assembly;

import lombok.Data;

import java.time.Instant;
import java.util.List;
import java.util.UUID;

@Data
public class AssemblyOrderResponse {
    private UUID id;
    private UUID sourceOrderId;
    private String sourceOrderType;
    private UUID carId;
    private UUID carModelId;
    private List<UUID> requiredPartIds;
    private UUID warehouseOperatorId;
    private String status;
    private boolean removed;
    private Instant createdAt;
    private Instant updatedAt;
}
