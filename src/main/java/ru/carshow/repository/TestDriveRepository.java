package ru.carshow.repository;

import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.TestDriveRequestEntity;
import java.util.List;
import java.util.UUID;

public interface TestDriveRepository extends CrudRepository<TestDriveRequestEntity, UUID> {
    List<TestDriveRequestEntity> findByClientId(UUID clientId);
    List<TestDriveRequestEntity> findByCarId(UUID carId);
}