package ru.carshow.repository;

import ru.carshow.domain.model.car.CarBrand;
import ru.carshow.domain.model.car.CarModel;
import ru.carshow.domain.model.car.BodyType;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.math.BigDecimal;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public class CarModelRepository {
    private final Map<Long, CarModel> carModels = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public CarModel save(CarModel carModel) {
        if (carModel.getId() == null) {
            long newId = generateId();
            try {
                CarModel newCarModel = carModel.getClass()
                        .getDeclaredConstructor(Long.class, CarBrand.class, String.class, BodyType.class, BigDecimal.class)
                        .newInstance(newId, carModel.getBrand(), carModel.getName(), carModel.getBodyType(), carModel.getBasePrice());
                carModels.put(newId, newCarModel);
                return newCarModel;
            } catch (Exception e) {
                throw new RuntimeException("Не удалось создать копию модели автомобиля с новым ID", e);
            }
        } else {
            carModels.put(carModel.getId(), carModel);
            return carModel;
        }
    }
    
    public Optional<CarModel> findById(Long id) {
        return Optional.ofNullable(carModels.get(id));
    }
    
    public CarModel getById(Long id) {
        return findById(id)
            .orElseThrow(() -> new EntityNotFoundException("Модель автомобиля с ID " + id + " не найдена"));
    }
    
    public List<CarModel> findAll() {
        return new ArrayList<>(carModels.values());
    }
    
    public List<CarModel> findByBrand(CarBrand brand) {
        return carModels.values().stream()
            .filter(model -> model.getBrand() == brand)
            .collect(Collectors.toList());
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
}