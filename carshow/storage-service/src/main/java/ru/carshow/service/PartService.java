package ru.carshow.service;

import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.dto.Part;
import ru.carshow.entity.*;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.mapper.PartMapper;
import ru.carshow.entity.PartCategory; 
import ru.carshow.entity.CarModelEntity;
import ru.carshow.repository.*;

import java.math.BigDecimal;
import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Service
@RequiredArgsConstructor
@Transactional(readOnly = true)
public class PartService {
    
    private final PartRepository partRepository;
    private final CarModelRepository carModelRepository;
    private final PartMapper partMapper;
    
    @Transactional
    public Part createPart(String name, String category, BigDecimal price, 
                          String manufacturer, String partNumber, Integer quantity) {
        PartEntity entity = new PartEntity();
        entity.setName(name);
        entity.setCategory(PartCategory.valueOf(category.toUpperCase()));
        entity.setPrice(price);
        entity.setManufacturer(manufacturer);
        entity.setPartNumber(partNumber);
        entity.setQuantity(quantity);
        
        PartEntity saved = partRepository.save(entity);
        return partMapper.toResponse(saved);
    }
    
    public Part getPartById(UUID id) {
        PartEntity entity = partRepository.findById(id)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Part not found: " + id));
        return partMapper.toResponse(entity);
    }
    
    public List<Part> getAllParts() {
        return StreamSupport.stream(partRepository.findAll().spliterator(), false)
            .filter(e -> !e.isRemoved())
            .map(partMapper::toResponse)
            .collect(Collectors.toList());
    }
    
    public List<Part> getPartsByCategory(String category) {
        PartCategory cat = PartCategory.valueOf(category.toUpperCase());
        return StreamSupport.stream(partRepository.findByCategory(cat).spliterator(), false)
            .filter(e -> !e.isRemoved())
            .map(partMapper::toResponse)
            .collect(Collectors.toList());
    }
    
    @Transactional
    public Part updatePart(UUID id, BigDecimal price, Integer quantity) {
        PartEntity entity = partRepository.findById(id)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Part not found: " + id));
        if (price != null) entity.setPrice(price);
        if (quantity != null) entity.setQuantity(quantity);
        PartEntity updated = partRepository.save(entity);
        return partMapper.toResponse(updated);
    }
    
    @Transactional
    public void addCompatibility(UUID partId, UUID carModelId) {
        PartEntity part = partRepository.findById(partId)
            .orElseThrow(() -> new DomainValidationException("Part not found: " + partId));
        CarModelEntity model = carModelRepository.findById(carModelId)
            .orElseThrow(() -> new DomainValidationException("CarModel not found: " + carModelId));
        part.getCompatibleCarModels().add(model);
        partRepository.save(part);
    }
    
    @Transactional
    public void removeCompatibility(UUID partId, UUID carModelId) {
        PartEntity part = partRepository.findById(partId)
            .orElseThrow(() -> new DomainValidationException("Part not found: " + partId));
        CarModelEntity model = carModelRepository.findById(carModelId)
            .orElseThrow(() -> new DomainValidationException("CarModel not found: " + carModelId));
        part.getCompatibleCarModels().remove(model);
        partRepository.save(part);
    }
    
    public List<Part> findCompatibleParts(UUID carModelId) {
        return StreamSupport.stream(partRepository.findByCompatibleCarModelsId(carModelId).spliterator(), false)
            .filter(e -> !e.isRemoved())
            .map(partMapper::toResponse)
            .collect(Collectors.toList());
    }
    
    @Transactional
    public void deletePart(UUID id) {
        PartEntity entity = partRepository.findById(id)
            .filter(e -> !e.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Part not found: " + id));
        entity.setRemoved(true);
        partRepository.save(entity);
    }
}