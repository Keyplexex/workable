package ru.carshow.domain.model.user;

public class Client extends User {
    public Client(Long id, String name, String email, String phone) {
        super(id, name, email, phone);
    }

    @Override
    public String getRole() {
        return "CLIENT";
    }
}
