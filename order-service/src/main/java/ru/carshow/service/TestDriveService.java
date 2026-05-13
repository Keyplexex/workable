package ru.carshow.service;

import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.client.StorageInternalClient;
import ru.carshow.dto.TestDriveRequest;
import ru.carshow.entity.ClientEntity;
import ru.carshow.entity.TestDriveRequestEntity;
import ru.carshow.entity.TestDriveStatus;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.mapper.TestDriveMapper;
import ru.carshow.repository.TestDriveRepository;
import ru.carshow.repository.UserRepository;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Service
@RequiredArgsConstructor
@Transactional(readOnly = true)
public class TestDriveService {

    private final TestDriveRepository testDriveRepository;
    private final UserRepository userRepository;
    private final StorageInternalClient storageInternalClient;
    private final TestDriveMapper testDriveMapper;

    @Transactional
    public TestDriveRequest createRequest(UUID clientId, UUID carId, LocalDateTime startDateTime) {
        userRepository.findById(clientId)
                .filter(e -> !e.isRemoved())
                .orElseThrow(() -> new DomainValidationException("Client not found: " + clientId));

        if (!storageInternalClient.isCarAvailableForClientOperations(carId)) {
            throw new DomainValidationException("Car not found or not available: " + carId);
        }

        if (!isCarAvailableForTestDrive(carId, startDateTime)) {
            throw new DomainValidationException("Car is not available for test drive at this time");
        }

        TestDriveRequestEntity entity = new TestDriveRequestEntity();
        entity.setClient((ClientEntity) userRepository.findById(clientId).get());
        entity.setCarId(carId);
        entity.setStartDateTime(startDateTime);
        entity.setStatus(TestDriveStatus.PENDING);

        TestDriveRequestEntity saved = testDriveRepository.save(entity);
        return testDriveMapper.toResponse(saved);
    }

    public TestDriveRequest getRequestById(UUID id) {
        TestDriveRequestEntity entity = testDriveRepository.findById(id)
                .filter(e -> !e.isRemoved())
                .orElseThrow(() -> new DomainValidationException("TestDriveRequest not found: " + id));
        return testDriveMapper.toResponse(entity);
    }

    public List<TestDriveRequest> getAllRequests() {
        return StreamSupport.stream(testDriveRepository.findAll().spliterator(), false)
                .filter(e -> !e.isRemoved())
                .map(testDriveMapper::toResponse)
                .collect(Collectors.toList());
    }

    public List<TestDriveRequest> getRequestsByClient(UUID clientId) {
        return StreamSupport.stream(testDriveRepository.findByClientId(clientId).spliterator(), false)
                .filter(e -> !e.isRemoved())
                .map(testDriveMapper::toResponse)
                .collect(Collectors.toList());
    }

    public List<TestDriveRequest> getRequestsByCar(UUID carId) {
        return StreamSupport.stream(testDriveRepository.findByCarId(carId).spliterator(), false)
                .filter(e -> !e.isRemoved())
                .map(testDriveMapper::toResponse)
                .collect(Collectors.toList());
    }

    @Transactional
    public TestDriveRequest updateStatus(UUID requestId, TestDriveStatus newStatus) {
        TestDriveRequestEntity entity = testDriveRepository.findById(requestId)
                .orElseThrow(() -> new DomainValidationException("TestDriveRequest not found: " + requestId));
        entity.setStatus(newStatus);
        TestDriveRequestEntity updated = testDriveRepository.save(entity);
        return testDriveMapper.toResponse(updated);
    }

    @Transactional
    public TestDriveRequest confirmRequest(UUID requestId) {
        return updateStatus(requestId, TestDriveStatus.CONFIRMED);
    }

    @Transactional
    public TestDriveRequest completeRequest(UUID requestId) {
        return updateStatus(requestId, TestDriveStatus.COMPLETED);
    }

    @Transactional
    public TestDriveRequest cancelRequest(UUID requestId) {
        return updateStatus(requestId, TestDriveStatus.CANCELLED);
    }

    public boolean isCarAvailableForTestDrive(UUID carId, LocalDateTime dateTime) {
        return StreamSupport.stream(testDriveRepository.findByCarId(carId).spliterator(), false)
                .noneMatch(r -> !r.isRemoved()
                        && r.getStartDateTime().isEqual(dateTime)
                        && r.getStatus() != TestDriveStatus.CANCELLED
                        && r.getStatus() != TestDriveStatus.COMPLETED);
    }
}
