package ru.carshow.repository;

import org.springframework.data.jpa.repository.JpaSpecificationExecutor;
import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.CarEntity;
import java.util.Optional;
import java.util.UUID;

public interface CarRepository extends CrudRepository<CarEntity, UUID>, JpaSpecificationExecutor<CarEntity> {
    Optional<CarEntity> findByVin(String vin);
}