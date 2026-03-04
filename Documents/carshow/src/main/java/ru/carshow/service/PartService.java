package ru.carshow.service;

import ru.carshow.domain.model.part.Part;
import ru.carshow.domain.model.part.PartCategory;
import ru.carshow.repository.PartRepository;
import ru.carshow.repository.CarModelRepository;
import ru.carshow.domain.exception.DomainValidationException;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.math.BigDecimal;
import java.util.List;

public class PartService {
    private final PartRepository partRepository;
    private final CarModelRepository carModelRepository;
    
    public PartService(PartRepository partRepository, CarModelRepository carModelRepository) {
        this.partRepository = partRepository;
        this.carModelRepository = carModelRepository;
    }
    
    public Part createPart(String name, PartCategory category, BigDecimal price,
                          String manufacturer, String partNumber, int quantity) {
        
        validatePartData(name, price, manufacturer, partNumber, quantity);
        
        Long id = partRepository.generateId();
        Part part = new Part(id, name, category, price, manufacturer, partNumber, quantity);
        
        return partRepository.save(part);
    }
    
    public Part getPartById(Long id) {
        return partRepository.getById(id);
    }
    
    public List<Part> getAllParts() {
        return partRepository.findAll();
    }
    
    public List<Part> getPartsByCategory(PartCategory category) {
        return partRepository.findByCategory(category);
    }
    
    public Part updatePart(Long id, BigDecimal price, Integer quantity) {
        Part part = partRepository.getById(id);
        
        if (price != null) {
            part.setPrice(price);
        }

        if (quantity != null && quantity >= 0) {
            part.setQuantity(quantity);
        }
        
        return partRepository.save(part);
    }
    
    public void addCompatibility(Long partId, Long carModelId) {
        Part part = partRepository.getById(partId);
        carModelRepository.getById(carModelId); 
        
        part.addCompatibleCarModel(carModelId);
        partRepository.save(part);
    }
    
    public void removeCompatibility(Long partId, Long carModelId) {
        Part part = partRepository.getById(partId);
        part.removeCompatibleCarModel(carModelId);
        partRepository.save(part);
    }
    
    public List<Part> findCompatibleParts(Long carModelId) {
        carModelRepository.getById(carModelId); 
        return partRepository.findCompatibleWithCarModel(carModelId);
    }
    
    public void deletePart(Long id) {
        if (!partRepository.findById(id).isPresent()) {
            throw new EntityNotFoundException("Запчасть с ID " + id + " не найдена");
        }
        partRepository.delete(id);
    }
    
    private void validatePartData(String name, BigDecimal price, 
                                 String manufacturer, String partNumber, int quantity) {
        if (name == null || name.trim().isEmpty()) {
            throw new DomainValidationException("Название запчасти не может быть пустым");
        }
        if (price == null) {
            throw new DomainValidationException("Цена должна быть указана");
        }
        if (manufacturer == null || manufacturer.trim().isEmpty()) {
            throw new DomainValidationException("Производитель не может быть пустым");
        }
        if (partNumber == null || partNumber.trim().isEmpty()) {
            throw new DomainValidationException("Номер запчасти не может быть пустым");
        }
        if (quantity < 0) {
            throw new DomainValidationException("Количество не может быть отрицательным");
        }
    }
}