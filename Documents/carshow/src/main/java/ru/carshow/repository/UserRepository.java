package ru.carshow.repository;

import ru.carshow.domain.model.user.User;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

public class UserRepository {
    private final Map<Long, User> users = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public User save(User user) {
        if (user.getId() == null) {
            long newId = generateId();
            try {
                User newUser = user.getClass()
                        .getDeclaredConstructor(Long.class, String.class, String.class, String.class)
                        .newInstance(newId, user.getName(), user.getEmail(), user.getPhone());
                users.put(newId, newUser);
                return newUser;
            } catch (Exception e) {
                throw new RuntimeException("Не удалось создать копию пользователя с новым ID", e);
            }
        } else {
            users.put(user.getId(), user);
            return user;
        }
    }
    
    public Optional<User> findById(Long id) {
        return Optional.ofNullable(users.get(id));
    }
    
    public User getById(Long id) {
        return findById(id)
                .orElseThrow(() -> new EntityNotFoundException("Пользователь с ID " + id + " не найден"));
    }
    
    public List<User> findAll() {
        return new ArrayList<>(users.values());
    }
    
    public void delete(Long id) {
        users.remove(id);
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
}