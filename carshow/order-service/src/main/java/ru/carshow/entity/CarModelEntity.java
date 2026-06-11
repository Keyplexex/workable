package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.math.BigDecimal;
import java.util.List;
import java.util.UUID;

@Getter @Setter @Entity @Table(name = "car_models")
public class CarModelEntity extends BaseEntity {
    
    private String brand;
    private String name;
    private BigDecimal basePrice;
    private String bodyType; 
    
    @OneToMany(mappedBy = "carModel", cascade = CascadeType.ALL, orphanRemoval = true)
    private List<ComponentEntity> components;
    
    @ManyToMany
    @JoinTable(name = "components_compatible_models",
               joinColumns = @JoinColumn(name = "car_model_id"),
               inverseJoinColumns = @JoinColumn(name = "component_id"))
    private List<ComponentEntity> compatibleComponents;
}