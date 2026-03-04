package ru.carshow.repository;

import ru.carshow.domain.model.testdrive.TestDriveRequest;
import ru.carshow.domain.exception.EntityNotFoundException;

import java.time.LocalDateTime;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public class TestDriveRepository {
    private final Map<Long, TestDriveRequest> requests = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);
    
    public TestDriveRequest save(TestDriveRequest request) {
        if (request.getId() == null) {
            long newId = generateId();
            
            try {
                TestDriveRequest newRequest = new TestDriveRequest(
                    newId,
                    request.getClient(),
                    request.getCar(),
                    request.getStartDateTime()
                );
                
                if (request.getStatus() != TestDriveRequest.TestDriveStatus.PENDING) {
                    newRequest.setStatus(request.getStatus());
                }
                
                requests.put(newId, newRequest);
                return newRequest;
            } catch (Exception e) {
                throw new RuntimeException("Не удалось создать копию заявки на тест-драйв с новым ID", e);
            }
        } else {
            requests.put(request.getId(), request);
            return request;
        }
    }
    
    public Optional<TestDriveRequest> findById(Long id) {
        return Optional.ofNullable(requests.get(id));
    }
    
    public TestDriveRequest getById(Long id) {
        return findById(id)
            .orElseThrow(() -> new EntityNotFoundException("Заявка на тест-драйв с ID " + id + " не найдена"));
    }
    
    public List<TestDriveRequest> findAll() {
        return new ArrayList<>(requests.values());
    }
    
    public List<TestDriveRequest> findByClientId(Long clientId) {
        return requests.values().stream()
            .filter(request -> request.getClient().getId().equals(clientId))
            .collect(Collectors.toList());
    }
    
    public List<TestDriveRequest> findByCarId(Long carId) {
        return requests.values().stream()
            .filter(request -> request.getCar().getId().equals(carId))
            .collect(Collectors.toList());
    }
    
    public List<TestDriveRequest> findByDate(LocalDateTime date) {
        return requests.values().stream()
            .filter(request -> request.getStartDateTime().toLocalDate().equals(date.toLocalDate()))
            .collect(Collectors.toList());
    }
    
    public long generateId() {
        return idGenerator.getAndIncrement();
    }
}