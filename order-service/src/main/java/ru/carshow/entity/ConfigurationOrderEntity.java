package ru.carshow.entity;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import jakarta.persistence.Column;
import jakarta.persistence.DiscriminatorValue;
import jakarta.persistence.Entity;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.ManyToOne;
import lombok.Getter;
import lombok.Setter;

import java.math.BigDecimal;
import java.util.Collections;
import java.util.Map;
import java.util.UUID;

@Getter
@Setter
@Entity
@DiscriminatorValue("ConfigurationOrder")
public class ConfigurationOrderEntity extends OrderEntity {

    private static final ObjectMapper MAPPER = new ObjectMapper();

    @ManyToOne
    @JoinColumn(name = "car_model_id")
    private CarModelEntity carModel;

    @Column(name = "selected_components", columnDefinition = "jsonb")
    private String selectedComponentsJson;

    private BigDecimal finalPrice;

    public ConfigurationOrderEntity() {}

    public Map<String, UUID> getSelectedComponents() {
        if (selectedComponentsJson == null || selectedComponentsJson.isBlank()) {
            return Collections.emptyMap();
        }
        try {
            return MAPPER.readValue(selectedComponentsJson, new TypeReference<>() {});
        } catch (Exception e) {
            throw new IllegalStateException("Invalid selected_components json", e);
        }
    }

    public void setSelectedComponents(Map<String, UUID> components) {
        try {
            this.selectedComponentsJson = components == null || components.isEmpty()
                    ? "{}"
                    : MAPPER.writeValueAsString(components);
        } catch (Exception e) {
            throw new IllegalStateException("Cannot serialize selected_components", e);
        }
    }
}
