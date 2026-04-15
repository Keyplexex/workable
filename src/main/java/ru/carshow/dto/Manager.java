package ru.carshow.dto;

import java.util.UUID;

public class Manager extends User {
    public Manager() { super(); }
    
    public Manager(UUID id, String name, String email, String phone) {
        super(id, name, email, phone);
    }
}