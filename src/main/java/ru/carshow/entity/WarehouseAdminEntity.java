package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@DiscriminatorValue("WarehouseAdmin")
public class WarehouseAdminEntity extends UserEntity {
    public WarehouseAdminEntity() {}
}