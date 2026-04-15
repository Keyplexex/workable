package ru.carshow.entity;

import ru.carshow.entity.CarStatus;
import ru.carshow.entity.FuelType;
import ru.carshow.entity.TransmissionType;
import ru.carshow.entity.DriveType;
import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import java.math.BigDecimal;
import java.util.UUID;

@Entity
@Table(name = "cars")
@Getter
@Setter
public class CarEntity extends BaseEntity {
    
    @ManyToOne
    @JoinColumn(name = "model_id", nullable = false)
    private CarModelEntity model;
    
    @Column(nullable = false, unique = true, length = 17)
    private String vin;
    
    @Column(nullable = false)
    private int year;
    
    private String color;
    
    @Column(name = "engine_power")
    private BigDecimal enginePower;
    
    @Column(name = "engine_volume")
    private BigDecimal engineVolume;
    
    @Enumerated(EnumType.STRING)
    private FuelType fuelType;          
    
    @Enumerated(EnumType.STRING)
    private TransmissionType transmission; 
    
    @Enumerated(EnumType.STRING)
    private DriveType driveType;      
    
    @Enumerated(EnumType.STRING)
    private CarStatus status;      
    
    private BigDecimal price;
}