package ru.carshow.domain.model.user;

public class WarehouseAdmin extends User{
    public WarehouseAdmin(Long id, String name, String email, String phone) {
        super(id, name, email, phone);
    }

    @Override 
    public String getRole() {
        return "WAREHOUSE_ADMIN";
    }
}
