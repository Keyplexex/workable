package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@DiscriminatorValue("Manager")
public class ManagerEntity extends UserEntity {
    public ManagerEntity() {}
}