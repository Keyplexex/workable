package ru.carshow.dto.response;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import java.math.BigDecimal;
import java.util.List;
import java.util.UUID;

@Data @NoArgsConstructor @AllArgsConstructor
public class ConfigurationResult {
    private UUID carModelId;
    private String carModelName;
    private BigDecimal basePrice;
    private List<ComponentDto> selectedComponents;
    private BigDecimal componentsSupplement;
    private BigDecimal finalPrice;
    
    @Data @NoArgsConstructor @AllArgsConstructor
    public static class ComponentDto {
        private UUID id;
        private String type;
        private String name;
        private BigDecimal priceSupplement;
    }
}