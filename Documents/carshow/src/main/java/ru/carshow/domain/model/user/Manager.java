package ru.carshow.domain.model.user;

public class Manager extends User{
    public Manager(Long id, String name, String email, String phone) {
        super(id, name, email, phone);
    }

    @Override 
    public String getRole() {
        return "MANAGER";
    }
}
