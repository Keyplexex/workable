package ru.carshow.service;

import ru.carshow.dto.Client;
import ru.carshow.dto.User;
import ru.carshow.entity.ClientEntity;
import ru.carshow.entity.UserEntity;
import ru.carshow.repository.UserRepository;
import ru.carshow.mapper.UserMapper;
import ru.carshow.exception.DomainValidationException;

import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

public class UserService {
    private final UserRepository userRepository;
    private final UserMapper userMapper;
    
    public UserService(UserRepository userRepository, UserMapper userMapper) {
        this.userRepository = userRepository;
        this.userMapper = userMapper;
    }
    
    public Client createClient(String name, String email, String phone) {
        validateUserData(name, email, phone);
        
        ClientEntity entity = new ClientEntity();
        entity.setName(name);
        entity.setEmail(email);
        entity.setPhone(phone);
        
        ClientEntity saved = userRepository.save(entity);
        return userMapper.toResponse(saved);
    }
    
    public User getUserById(UUID id) {
        UserEntity entity = userRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("User not found: " + id));
        return userMapper.toResponse(entity);
    }
    
    public List<User> getAllUsers() {
        return StreamSupport.stream(userRepository.findAll().spliterator(), false)
            .map(userMapper::toResponse)
            .collect(Collectors.toList());
    }
    
    public User updateUser(UUID id, String name, String email, String phone) {
        UserEntity entity = userRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("User not found: " + id));
        
        if (name != null && !name.trim().isEmpty()) entity.setName(name);
        if (email != null && !email.trim().isEmpty()) entity.setEmail(email);
        if (phone != null && !phone.trim().isEmpty()) entity.setPhone(phone);
        
        UserEntity updated = userRepository.save(entity);
        return userMapper.toResponse(updated);
    }
    
    public void deleteUser(UUID id) {
        UserEntity entity = userRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("User not found: " + id));
        entity.setRemoved(true);
        userRepository.save(entity);
    }
    
    private void validateUserData(String name, String email, String phone) {
        if (name == null || name.trim().isEmpty())
            throw new DomainValidationException("Name cannot be empty");
        if (email == null || !email.matches("^[A-Za-z0-9+_.-]+@(.+)$"))
            throw new DomainValidationException("Invalid email");
        if (phone == null || phone.trim().isEmpty())
            throw new DomainValidationException("Phone cannot be empty");
    }
}