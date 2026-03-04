package ru.carshow.domain.model.user;

public class SystemAdmin extends User{
    public SystemAdmin(Long id, String name, String email, String phone) {
        super(id, name, email, phone);
    }

    @Override 
    public String getRole() {
        return "SYSTEM_ADMIN";
    }
}
