package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.math.BigDecimal;
import java.util.UUID;

@Getter @Setter
@Entity
@DiscriminatorValue("ConfigurationOrder")
public class ConfigurationOrderEntity extends OrderEntity {
    
    @ManyToOne @JoinColumn(name = "car_model_id")
    private CarModelEntity carModel;
    
    @Column(name = "selected_components", columnDefinition = "jsonb")
    private String selectedComponentsJson;
    
    private BigDecimal finalPrice;
    
    public ConfigurationOrderEntity() {}
    
    public java.util.Map<String, UUID> getSelectedComponents() { return null; }
    public void setSelectedComponents(java.util.Map<String, UUID> components) { 
        this.selectedComponentsJson = "{}"; 
    }
}