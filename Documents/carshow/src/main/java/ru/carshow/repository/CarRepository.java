package ru.carshow.repository;

import ru.carshow.domain.model.car.*;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.math.BigDecimal;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public class CarRepository {
    private final Map<Long, Car> cars = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public Car save(Car car) {
        if (car.getId() == null) {
            long newId = generateId();
            try {
                Car newCar = car.getClass()
                        .getDeclaredConstructor(Long.class, CarModel.class, String.class, int.class, String.class,
                                double.class, double.class, FuelType.class, TransmissionType.class, DriveType.class,
                                CarStatus.class, BigDecimal.class)
                        .newInstance(newId, car.getModel(), car.getVin(), car.getYear(), car.getColor(),
                                car.getEnginePower(), car.getEngineVolume(), car.getFuelType(),
                                car.getTransmissionType(), car.getDriveType(), car.getStatus(), car.getPrice());
                cars.put(newId, newCar);
                return newCar;
            } catch (Exception e) {
                throw new RuntimeException("Не удалось создать копию автомобиля с новым ID", e);
            }
        } else {
            cars.put(car.getId(), car);
            return car;
        }
    }
    
    public Optional<Car> findById(Long id) {
        return Optional.ofNullable(cars.get(id));
    }
    
    public Car getById(Long id) {
        return findById(id)
            .orElseThrow(() -> new EntityNotFoundException("Автомобиль с ID " + id + " не найден"));
    }
    
    public List<Car> findAll() {
        return new ArrayList<>(cars.values());
    }
    
    public List<Car> findAvailable() {
        return cars.values().stream()
            .filter(car -> car.getStatus() == CarStatus.AVAILABLE)
            .collect(Collectors.toList());
    }
    
    public List<Car> findForTestDrive() {
        return cars.values().stream()
            .filter(car -> car.getStatus() == CarStatus.FOR_TEST_DRIVE)
            .collect(Collectors.toList());
    }
    
    public List<Car> findByFilters(CarFilter filter) {
        return cars.values().stream()
            .filter(car -> filter.matches(car))
            .collect(Collectors.toList());
    }
    
    public void delete(Long id) {
        cars.remove(id);
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
    
    public static class CarFilter {
        private BigDecimal minPrice;
        private BigDecimal maxPrice;
        private CarBrand brand;
        private String modelName;
        private BodyType bodyType;
        private FuelType fuelType;
        private Double minEnginePower;
        private Double maxEnginePower;
        private Double minEngineVolume;
        private Double maxEngineVolume;
        private TransmissionType transmission;
        private DriveType driveType;
        private String color;
        
        public boolean matches(Car car) {
            boolean matches = true;
            
            if (minPrice != null && car.getPrice().compareTo(minPrice) < 0) matches = false;
            if (maxPrice != null && car.getPrice().compareTo(maxPrice) > 0) matches = false;
            if (brand != null && car.getModel().getBrand() != brand) matches = false;
            if (modelName != null && !car.getModel().getName().equalsIgnoreCase(modelName)) matches = false;
            if (bodyType != null && car.getModel().getBodyType() != bodyType) matches = false;
            if (fuelType != null && car.getFuelType() != fuelType) matches = false;
            if (minEnginePower != null && car.getEnginePower() < minEnginePower) matches = false;
            if (maxEnginePower != null && car.getEnginePower() > maxEnginePower) matches = false;
            if (minEngineVolume != null && car.getEngineVolume() < minEngineVolume) matches = false;
            if (maxEngineVolume != null && car.getEngineVolume() > maxEngineVolume) matches = false;
            if (transmission != null && car.getTransmissionType() != transmission) matches = false;
            if (driveType != null && car.getDriveType() != driveType) matches = false;
            if (color != null && !car.getColor().equalsIgnoreCase(color)) matches = false;
            
            return matches;
        }
        
        public CarFilter withMinPrice(BigDecimal minPrice) { 
            this.minPrice = minPrice; return this; 
        }
        public CarFilter withMaxPrice(BigDecimal maxPrice) { 
            this.maxPrice = maxPrice; return this; 
        }
        public CarFilter withBrand(CarBrand brand) { 
            this.brand = brand; return this; 
        }
        public CarFilter withModelName(String modelName) { 
            this.modelName = modelName; return this; 
        }
        public CarFilter withBodyType(BodyType bodyType) { 
            this.bodyType = bodyType; return this; 
        }
        public CarFilter withFuelType(FuelType fuelType) { 
            this.fuelType = fuelType; return this; 
        }
        public CarFilter withMinEnginePower(Double minEnginePower) { 
            this.minEnginePower = minEnginePower; return this; 
        }
        public CarFilter withMaxEnginePower(Double maxEnginePower) { 
            this.maxEnginePower = maxEnginePower; return this; 
        }
        public CarFilter withMinEngineVolume(Double minEngineVolume) { 
            this.minEngineVolume = minEngineVolume; return this; 
        }
        public CarFilter withMaxEngineVolume(Double maxEngineVolume) { 
            this.maxEngineVolume = maxEngineVolume; return this; 
        }
        public CarFilter withTransmission(TransmissionType transmission) { 
            this.transmission = transmission; return this; 
        }
        public CarFilter withDriveType(DriveType driveType) { 
            this.driveType = driveType; return this; 
        }
        public CarFilter withColor(String color) { 
            this.color = color; return this; 
        }
    }
}