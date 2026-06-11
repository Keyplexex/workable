package ru.carshow.dto.request;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import java.util.Map;
import java.util.UUID;

@Data @NoArgsConstructor @AllArgsConstructor
public class BuildConfigurationRequest {
    private UUID carModelId;
    private Map<String, UUID> selectedComponents;
}