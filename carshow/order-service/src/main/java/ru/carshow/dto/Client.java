package ru.carshow.dto;

import java.util.UUID;

public class Client extends User {
    public Client() { super(); }
    
    public Client(UUID id, String name, String email, String phone) {
        super(id, name, email, phone);
    }
}