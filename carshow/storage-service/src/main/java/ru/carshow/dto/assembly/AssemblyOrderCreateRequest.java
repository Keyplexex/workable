package ru.carshow.dto.assembly;

import lombok.Data;

import java.util.List;
import java.util.UUID;

@Data
public class AssemblyOrderCreateRequest {
    private UUID sourceOrderId;
    private String sourceOrderType;
    private UUID carId;
    private UUID carModelId;
    private List<UUID> requiredPartIds;
    private UUID warehouseOperatorId;
    private String status;
}
