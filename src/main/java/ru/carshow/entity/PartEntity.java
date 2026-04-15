package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

@Getter @Setter @Entity @Table(name = "parts")
public class PartEntity extends BaseEntity {
    
    private String name;
    
    @Enumerated(EnumType.STRING)
    private PartCategory category;
    
    private BigDecimal price;
    private String manufacturer;
    private String partNumber;
    private Integer quantity;
    
    @ManyToMany
    @JoinTable(
        name = "parts_compatible_models",
        joinColumns = @JoinColumn(name = "part_id"),
        inverseJoinColumns = @JoinColumn(name = "car_model_id")
    )
    private List<CarModelEntity> compatibleCarModels = new ArrayList<>();
    
    public PartEntity() {}
}