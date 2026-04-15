package ru.carshow.mapper;

import org.springframework.stereotype.Component;
import ru.carshow.dto.CarResponse;
import ru.carshow.dto.CreateCarRequest;
import ru.carshow.dto.UpdateCarRequest;
import ru.carshow.entity.CarEntity;
import ru.carshow.entity.CarModelEntity;
import ru.carshow.entity.CarStatus; 

@Component
public class CarMapper {
    
    public CarResponse toResponse(CarEntity car) {
        if (car == null) return null;
        CarResponse dto = new CarResponse();
        dto.setId(car.getId());
        dto.setVin(car.getVin());
        dto.setYear(car.getYear());
        dto.setColor(car.getColor());
        
        dto.setEnginePower(car.getEnginePower());
        dto.setEngineVolume(car.getEngineVolume());
        
        dto.setFuelType(car.getFuelType() != null ? car.getFuelType().name() : null);
        dto.setTransmission(car.getTransmission() != null ? car.getTransmission().name() : null);
        dto.setDriveType(car.getDriveType() != null ? car.getDriveType().name() : null);
        dto.setPrice(car.getPrice());
        dto.setStatus(car.getStatus() != null ? car.getStatus().name() : null);
        
        if (car.getModel() != null) {
            dto.setModelId(car.getModel().getId());
            dto.setModelBrand(car.getModel().getBrand());
            dto.setModelName(car.getModel().getName());
        }
        return dto;
    }
    
    public CarEntity toEntity(CreateCarRequest request) {
        if (request == null) return null;
        CarEntity entity = new CarEntity();
        entity.setVin(request.getVin());
        entity.setYear(request.getYear());
        entity.setColor(request.getColor());
        
        entity.setEnginePower(request.getEnginePower());
        entity.setEngineVolume(request.getEngineVolume());
        
        entity.setPrice(request.getPrice());
        return entity;
    }
    
    public void updateEntity(CarEntity car, UpdateCarRequest request) {
        if (request == null || car == null) return;
        if (request.getColor() != null) car.setColor(request.getColor());
        if (request.getPrice() != null) car.setPrice(request.getPrice());
        
        if (request.getStatus() != null) {
            try {
                car.setStatus(CarStatus.valueOf(request.getStatus().toUpperCase()));
            } catch (IllegalArgumentException e) {
            }
        }
    }
}