package ru.carshow.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Getter @Setter
@Entity
@Table(name = "users")  
@Inheritance(strategy = InheritanceType.SINGLE_TABLE) 
@DiscriminatorColumn(name = "dtype", discriminatorType = DiscriminatorType.STRING)  
public abstract class UserEntity extends BaseEntity {
    
    private String name;
    private String email;
    private String phone;
    
    public UserEntity() {}
}