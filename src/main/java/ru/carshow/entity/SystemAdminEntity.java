package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@DiscriminatorValue("SystemAdmin")
public class SystemAdminEntity extends UserEntity {
    public SystemAdminEntity() {}
}