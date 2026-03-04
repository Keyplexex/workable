package ru.carshow.service;

import ru.carshow.domain.model.car.Car;
import ru.carshow.domain.model.testdrive.TestDriveRequest;
import ru.carshow.domain.model.user.Client;
import ru.carshow.repository.TestDriveRepository;
import ru.carshow.repository.UserRepository;
import ru.carshow.repository.CarRepository;
import ru.carshow.domain.exception.DomainValidationException;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.time.LocalDateTime;
import java.util.List;

public class TestDriveService {
    private final TestDriveRepository testDriveRepository;
    private final UserRepository userRepository;
    private final CarRepository carRepository;
    
    public TestDriveService(TestDriveRepository testDriveRepository, 
                           UserRepository userRepository,
                           CarRepository carRepository) {
        this.testDriveRepository = testDriveRepository;
        this.userRepository = userRepository;
        this.carRepository = carRepository;
    }
    
    public TestDriveRequest createRequest(Long clientId, Long carId, LocalDateTime startDateTime) {
        Client client = (Client) userRepository.getById(clientId);
        Car car = carRepository.getById(carId);
        
        validateRequestData(client, car, startDateTime);
        
        Long id = testDriveRepository.generateId();
        TestDriveRequest request = new TestDriveRequest(id, client, car, startDateTime);
        
        return testDriveRepository.save(request);
    }
    
    public TestDriveRequest getRequestById(Long id) {
        return testDriveRepository.getById(id);
    }
    
    public List<TestDriveRequest> getAllRequests() {
        return testDriveRepository.findAll();
    }
    
    public List<TestDriveRequest> getRequestsByClient(Long clientId) {
        return testDriveRepository.findByClientId(clientId);
    }
    
    public List<TestDriveRequest> getRequestsByCar(Long carId) {
        return testDriveRepository.findByCarId(carId);
    }
    
    public TestDriveRequest confirmRequest(Long requestId) {
        TestDriveRequest request = testDriveRepository.getById(requestId);
        request.setStatus(TestDriveRequest.TestDriveStatus.CONFIRMED);
        return testDriveRepository.save(request);
    }
    
    public TestDriveRequest completeRequest(Long requestId) {
        TestDriveRequest request = testDriveRepository.getById(requestId);
        request.setStatus(TestDriveRequest.TestDriveStatus.COMPLETED);
        return testDriveRepository.save(request);
    }
    
    public TestDriveRequest cancelRequest(Long requestId) {
        TestDriveRequest request = testDriveRepository.getById(requestId);
        
        if (request.getStartDateTime().isBefore(LocalDateTime.now())) {
            throw new DomainValidationException("Нельзя отменить уже прошедший тест-драйв");
        }
        
        request.setStatus(TestDriveRequest.TestDriveStatus.CANCELLED);
        return testDriveRepository.save(request);
    }
    
    public boolean isCarAvailableForTestDrive(Long carId, LocalDateTime dateTime) {
        List<TestDriveRequest> requests = testDriveRepository.findByCarId(carId);
        
        return requests.stream()
            .filter(r -> r.getStatus() == TestDriveRequest.TestDriveStatus.CONFIRMED)
            .noneMatch(r -> isOverlapping(r.getStartDateTime(), dateTime));
    }
    
    private void validateRequestData(Client client, Car car, LocalDateTime startDateTime) {
        if (startDateTime.isBefore(LocalDateTime.now())) {
            throw new DomainValidationException("Дата тест-драйва не может быть в прошлом");
        }
        
        if (!isCarAvailableForTestDrive(car.getId(), startDateTime)) {
            throw new DomainValidationException("Автомобиль уже забронирован на это время");
        }
    }
    
    private boolean isOverlapping(LocalDateTime existing, LocalDateTime newTime) {
        return Math.abs(existing.getHour() - newTime.getHour()) < 1 && 
               existing.toLocalDate().equals(newTime.toLocalDate());
    }
}