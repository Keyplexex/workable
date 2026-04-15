package ru.carshow.mapper;

import org.springframework.stereotype.Component;
import ru.carshow.dto.User;
import ru.carshow.dto.Client;
import ru.carshow.entity.UserEntity;
import ru.carshow.entity.ClientEntity;

@Component
public class UserMapper {
    
    public User toResponse(UserEntity entity) {
        if (entity == null) return null;
        User dto = new User();
        dto.setId(entity.getId());
        dto.setName(entity.getName());
        dto.setEmail(entity.getEmail());
        dto.setPhone(entity.getPhone());
        return dto;
    }
    
    public Client toResponse(ClientEntity entity) {
        if (entity == null) return null;
        Client dto = new Client();
        dto.setId(entity.getId());
        dto.setName(entity.getName());
        dto.setEmail(entity.getEmail());
        dto.setPhone(entity.getPhone());
        return dto;
    }
}