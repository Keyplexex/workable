package ru.carshow.service;

import ru.carshow.domain.model.car.*;
import ru.carshow.repository.CarRepository;
import ru.carshow.repository.CarModelRepository;
import ru.carshow.domain.exception.DomainValidationException;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.math.BigDecimal;
import java.util.List;

public class CarService {
    private final CarRepository carRepository;
    private final CarModelRepository carModelRepository;
    
    public CarService(CarRepository carRepository, CarModelRepository carModelRepository) {
        this.carRepository = carRepository;
        this.carModelRepository = carModelRepository;
    }

    public List<CarModel> getCarModels() {
        return carModelRepository.findAll();
    }
    
    public Car createCar(Long modelId, String vin, int year, String color, 
                        double enginePower, double engineVolume, FuelType fuelType,
                        TransmissionType transmission, DriveType driveType, 
                        BigDecimal price) {
        
        CarModel model = carModelRepository.getById(modelId);
        
        validateCarData(vin, year, color, enginePower, engineVolume, price);
        
        Long id = carRepository.generateId();
        Car car = new Car(id, model, vin, year, color, enginePower, engineVolume,
                         fuelType, transmission, driveType, CarStatus.AVAILABLE, price);
        
        return carRepository.save(car);
    }
    
    public Car getCarById(Long id) {
        return carRepository.getById(id);
    }
    
    public List<Car> getAllCars() {
        return carRepository.findAll();
    }
    
    public List<Car> getAvailableCars() {
        return carRepository.findAvailable();
    }
    
    public List<Car> getCarsForTestDrive() {
        return carRepository.findForTestDrive();
    }
    
    public List<Car> findCarsByFilters(CarRepository.CarFilter filter) {
        return carRepository.findByFilters(filter);
    }
    
    public Car updateCar(Long id, String color, BigDecimal price, CarStatus status) {
        Car car = carRepository.getById(id);
        
        if (color != null && !color.trim().isEmpty()) {
            car.setColor(color);
        }
        if (price != null && price.compareTo(BigDecimal.ZERO) > 0) {
            car.setPrice(price);
        }
        if (status != null) {
            car.setStatus(status);
        }
        
        return carRepository.save(car);
    }
    
    public void addCarToTestDrive(Long carId) {
        Car car = carRepository.getById(carId);
        car.setStatus(CarStatus.FOR_TEST_DRIVE);
        carRepository.save(car);
    }
    
    public void removeCarFromTestDrive(Long carId) {
        Car car = carRepository.getById(carId);
        if (car.getStatus() == CarStatus.FOR_TEST_DRIVE) {
            car.setStatus(CarStatus.AVAILABLE);
            carRepository.save(car);
        }
    }
    
    public void deleteCar(Long id) {
        if (!carRepository.findById(id).isPresent()) {
            throw new EntityNotFoundException("Автомобиль с ID " + id + " не найден");
        }
        carRepository.delete(id);
    }
    
    private void validateCarData(String vin, int year, String color, 
                                double enginePower, double engineVolume, BigDecimal price) {
        if (vin == null || vin.length() != 17) {
            throw new DomainValidationException("VIN должен содержать 17 символов");
        }
        if (year < 2000 || year > 2025) {
            throw new DomainValidationException("Некорректный год выпуска");
        }
        if (color == null || color.trim().isEmpty()) {
            throw new DomainValidationException("Цвет не может быть пустым");
        }
        if (enginePower <= 0) {
            throw new DomainValidationException("Мощность двигателя должна быть положительной");
        }
        if (engineVolume <= 0) {
            throw new DomainValidationException("Объем двигателя должен быть положительным");
        }
        if (price == null || price.compareTo(BigDecimal.ZERO) <= 0) {
            throw new DomainValidationException("Цена должна быть положительной");
        }
    }
}