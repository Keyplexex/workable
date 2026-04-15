package ru.carshow.service;

import org.springframework.transaction.annotation.Transactional;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import ru.carshow.dto.CarResponse;
import ru.carshow.dto.CreateCarRequest;
import ru.carshow.dto.UpdateCarRequest;
import ru.carshow.dto.CarFilterRequest;
import ru.carshow.entity.CarEntity;
import ru.carshow.entity.CarModelEntity;
import ru.carshow.repository.CarRepository;
import ru.carshow.repository.CarModelRepository;
import ru.carshow.mapper.CarMapper;
import ru.carshow.specification.CarSpecification;
import org.springframework.data.jpa.domain.Specification;

import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Service
@RequiredArgsConstructor
public class CarService {
    
    private final CarRepository carRepository;
    private final CarModelRepository carModelRepository;
    private final CarMapper carMapper;
    
    public CarResponse createCar(CreateCarRequest request) {
        CarModelEntity model = carModelRepository.findById(request.getModelId())
            .orElseThrow(() -> new RuntimeException("CarModel not found"));
        
        CarEntity entity = carMapper.toEntity(request);
        entity.setModel(model);
        
        CarEntity saved = carRepository.save(entity);
        return enrichCarResponse(carMapper.toResponse(saved));
    }
    
    public CarResponse getCarById(UUID id) {
        CarEntity entity = carRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("Car not found"));
        return enrichCarResponse(carMapper.toResponse(entity));
    }
    
    public List<CarResponse> getAllCars() {
        return StreamSupport.stream(carRepository.findAll().spliterator(), false)
            .map(carMapper::toResponse)
            .map(this::enrichCarResponse)
            .collect(Collectors.toList());
    }
    
    public CarResponse updateCar(UUID id, UpdateCarRequest request) {
        CarEntity entity = carRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("Car not found"));
        carMapper.updateEntity(entity, request);
        CarEntity updated = carRepository.save(entity);
        return enrichCarResponse(carMapper.toResponse(updated));
    }
    
    public void deleteCar(UUID id) {
        CarEntity entity = carRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("Car not found"));
        entity.setRemoved(true);
        carRepository.save(entity);
    }

    @Transactional
    public CarResponse deleteByID(UUID id) {
        CarEntity entity = carRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("Car not found"));
        entity.setRemoved(true);
        CarEntity saved = carRepository.save(entity);
        return enrichCarResponse(carMapper.toResponse(saved));
    }
    
    public List<CarResponse> getAvailableCars(CarFilterRequest filter) {
        Specification<CarEntity> spec = Specification.where(CarSpecification.isNotRemoved());
        
        if (filter.getBrand() != null) spec = spec.and(CarSpecification.hasBrand(filter.getBrand()));
        if (filter.getModel() != null) spec = spec.and(CarSpecification.hasModel(filter.getModel()));
        if (filter.getMinPrice() != null || filter.getMaxPrice() != null) 
            spec = spec.and(CarSpecification.priceBetween(filter.getMinPrice(), filter.getMaxPrice()));
        if (filter.getFuelType() != null) spec = spec.and(CarSpecification.hasFuelType(filter.getFuelType()));
        if (filter.getTransmission() != null) spec = spec.and(CarSpecification.hasTransmission(filter.getTransmission()));
        if (filter.getDriveType() != null) spec = spec.and(CarSpecification.hasDriveType(filter.getDriveType()));
        
        return StreamSupport.stream(carRepository.findAll(spec).spliterator(), false)
            .map(carMapper::toResponse)
            .map(this::enrichCarResponse)
            .collect(Collectors.toList());
    }
    
    private CarResponse enrichCarResponse(CarResponse dto) {
        if (dto.getModelId() != null) {
            CarModelEntity model = carModelRepository.findById(dto.getModelId()).orElse(null);
            if (model != null) {
                dto.setModelBrand(model.getBrand());
                dto.setModelName(model.getName());
            }
        }
        return dto;
    }
}