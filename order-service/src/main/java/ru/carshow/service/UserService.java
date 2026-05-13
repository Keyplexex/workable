package ru.carshow.service;

import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import lombok.RequiredArgsConstructor;

import ru.carshow.dto.Client;
import ru.carshow.dto.User;
import ru.carshow.entity.ClientEntity;
import ru.carshow.entity.SystemAdminEntity;
import ru.carshow.entity.UserEntity;
import ru.carshow.repository.UserRepository;
import ru.carshow.mapper.UserMapper;
import ru.carshow.exception.DomainValidationException;

import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Service                         
@RequiredArgsConstructor       
@Transactional(readOnly = true)
public class UserService {
    
    private final UserRepository userRepository;  
    private final UserMapper userMapper;        
    
    @Transactional
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
    
    private void validateUserData(String name, String email, String phone) {
        if (name == null || name.trim().isEmpty())
            throw new DomainValidationException("Name cannot be empty");
        if (email == null || !email.matches("^[A-Za-z0-9+_.-]+@(.+)$"))
            throw new DomainValidationException("Invalid email");
        if (phone == null || phone.trim().isEmpty())
            throw new DomainValidationException("Phone cannot be empty");
    }

    @Transactional
    public void deleteByID(UUID id) {
        UserEntity entity = userRepository.findById(id)
            .orElseThrow(() -> new DomainValidationException("User not found: " + id));
        if (entity instanceof SystemAdminEntity) {
            throw new DomainValidationException("Cannot delete system administrator");
        }
        entity.setRemoved(true);
        userRepository.save(entity);
    }
}