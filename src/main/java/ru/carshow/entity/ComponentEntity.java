package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.math.BigDecimal;
import java.util.List;
import java.util.UUID;

@Getter @Setter @Entity @Table(name = "components")
public class ComponentEntity extends BaseEntity {
    
    private String name;
    private String type; 
    private BigDecimal priceSupplement; 
    private boolean isDefault;
    
    @ManyToOne @JoinColumn(name = "car_model_id")
    private CarModelEntity carModel;
    
    @ManyToMany
    @JoinTable(name = "components_compatible_models",
               joinColumns = @JoinColumn(name = "component_id"),
               inverseJoinColumns = @JoinColumn(name = "car_model_id"))
    private List<CarModelEntity> compatibleCarModels;
    
    public boolean isCompatibleWith(CarModelEntity model) {
        return compatibleCarModels.stream().anyMatch(m -> m.getId().equals(model.getId()));
    }
}