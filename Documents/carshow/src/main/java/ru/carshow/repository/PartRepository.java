package ru.carshow.repository;

import ru.carshow.domain.model.part.Part;
import ru.carshow.domain.model.part.PartCategory;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public class PartRepository {
    private final Map<Long, Part> parts = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public Part save(Part part) {
        if (part.getId() == null) {
            long newId = generateId();
            
            Part newPart = new Part(
                newId,
                part.getName(),
                part.getCategory(),
                part.getPrice(),
                part.getManufacturer(),
                part.getPartNumber(),
                part.getQuantity()
            );
            
            for (Long modelId : part.getCompatibleCarModelIds()) {
                newPart.addCompatibleCarModel(modelId);
            }
            
            parts.put(newId, newPart);
            return newPart;
        } else {
            parts.put(part.getId(), part);
            return part;
        }
    }
    
    public Optional<Part> findById(Long id) {
        return Optional.ofNullable(parts.get(id));
    }
    
    public Part getById(Long id) {
        return findById(id)
            .orElseThrow(() -> new EntityNotFoundException("Запчасть с ID " + id + " не найдена"));
    }
    
    public List<Part> findAll() {
        return new ArrayList<>(parts.values());
    }
    
    public List<Part> findByCategory(PartCategory category) {
        return parts.values().stream()
            .filter(part -> part.getCategory() == category)
            .collect(Collectors.toList());
    }
    
    public List<Part> findCompatibleWithCarModel(Long carModelId) {
        return parts.values().stream()
            .filter(part -> part.isCompatibleWithCarModel(carModelId))
            .collect(Collectors.toList());
    }
    
    public void delete(Long id) {
        parts.remove(id);
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
}