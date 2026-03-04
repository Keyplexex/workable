package ru.carshow.service;

import ru.carshow.domain.model.user.*;
import ru.carshow.repository.UserRepository;
import ru.carshow.domain.exception.DomainValidationException;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.util.List;

public class UserService {
    private final UserRepository userRepository;
    
    public UserService(UserRepository userRepository) {
        this.userRepository = userRepository;
    }
    
    public Client createClient(String name, String email, String phone) {
        validateUserData(name, email, phone);
        
        Long id = userRepository.generateId();
        Client client = new Client(id, name, email, phone);
        return (Client) userRepository.save(client);
    }
    
    public Manager createManager(String name, String email, String phone) {
        validateUserData(name, email, phone);
        
        Long id = userRepository.generateId();
        Manager manager = new Manager(id, name, email, phone);
        return (Manager) userRepository.save(manager);
    }
    
    public WarehouseAdmin createWarehouseAdmin(String name, String email, String phone) {
        validateUserData(name, email, phone);
        
        Long id = userRepository.generateId();
        WarehouseAdmin admin = new WarehouseAdmin(id, name, email, phone);
        return (WarehouseAdmin) userRepository.save(admin);
    }
    
    public SystemAdmin createSystemAdmin(String name, String email, String phone) {
        validateUserData(name, email, phone);
        
        Long id = userRepository.generateId();
        SystemAdmin admin = new SystemAdmin(id, name, email, phone);
        return (SystemAdmin) userRepository.save(admin);
    }
    
    public User getUserById(Long id) {
        return userRepository.getById(id);
    }
    
    public List<User> getAllUsers() {
        return userRepository.findAll();
    }
    
    public User updateUser(Long id, String name, String email, String phone) {
        User user = userRepository.getById(id);
        
        if (name != null && !name.trim().isEmpty()) {
            user.setName(name);
        }
        if (email != null && !email.trim().isEmpty()) {
            user.setEmail(email);
        }
        if (phone != null && !phone.trim().isEmpty()) {
            user.setPhone(phone);
        }
        
        return userRepository.save(user);
    }
    
    public void deleteUser(Long id) {
        if (!userRepository.findById(id).isPresent()) {
            throw new EntityNotFoundException("Пользователь с ID " + id + " не найден");
        }
        userRepository.delete(id);
    }
    
    private void validateUserData(String name, String email, String phone) {
        if (name == null || name.trim().isEmpty()) {
            throw new DomainValidationException("Имя пользователя не может быть пустым");
        }
        if (email == null || !email.matches("^[A-Za-z0-9+_.-]+@(.+)$")) {
            throw new DomainValidationException("Некорректный формат email");
        }
        if (phone == null || phone.trim().isEmpty()) {
            throw new DomainValidationException("Телефон не может быть пустым");
        }
    }
}