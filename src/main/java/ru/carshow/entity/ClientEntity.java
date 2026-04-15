package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@DiscriminatorValue("Client") 
public class ClientEntity extends UserEntity {
    public ClientEntity() {}
}