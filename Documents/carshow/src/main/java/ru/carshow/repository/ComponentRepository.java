package ru.carshow.repository;

import ru.carshow.domain.model.configuration.Component;
import ru.carshow.domain.model.configuration.ComponentType;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public class ComponentRepository {
    private final Map<Long, Component> components = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public Component save(Component component) {
        if (component.getId() == null) {
            long newId = generateId();
            Component newComponent = new Component(
                newId,
                component.getName(),
                component.getType(),
                component.getDescription(),
                component.getSurcharge(),
                component.getPart(),
                component.isDefault()
            );
            components.put(newId, newComponent);
            return newComponent;
        } else {
            components.put(component.getId(), component);
            return component;
        }
    }
    
    public Optional<Component> findById(Long id) {
        return Optional.ofNullable(components.get(id));
    }
    
    public Component getById(Long id) {
        return findById(id)
            .orElseThrow(() -> new EntityNotFoundException("Компонент с ID " + id + " не найден"));
    }
    
    public List<Component> findAll() {
        return new ArrayList<>(components.values());
    }
    
    public List<Component> findByType(ComponentType type) {
        return components.values().stream()
            .filter(component -> component.getType() == type)
            .collect(Collectors.toList());
    }
    
    public List<Component> findCompatibleWithCarModel(Long carModelId) {
        return components.values().stream()
            .filter(component -> component.isCompatibleWithCarModel(carModelId))
            .collect(Collectors.toList());
    }
    
    public List<Component> findDefaultForCarModel(Long carModelId) {
        return components.values().stream()
            .filter(component -> component.isDefault() && 
                   component.isCompatibleWithCarModel(carModelId))
            .collect(Collectors.toList());
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
}