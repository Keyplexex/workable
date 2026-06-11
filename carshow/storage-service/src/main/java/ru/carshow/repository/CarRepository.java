package ru.carshow.repository;

import org.springframework.data.jpa.repository.JpaSpecificationExecutor;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.data.repository.query.Param;
import ru.carshow.entity.CarEntity;
import ru.carshow.entity.CarStatus;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

public interface CarRepository extends CrudRepository<CarEntity, UUID>, JpaSpecificationExecutor<CarEntity> {
    Optional<CarEntity> findByVin(String vin);

    @Query("SELECT DISTINCT c FROM CarEntity c JOIN FETCH c.model WHERE c.status = :status AND c.removed = false")
    List<CarEntity> findAvailableForSaleWithModel(@Param("status") CarStatus status);

    @Query("SELECT c FROM CarEntity c JOIN FETCH c.model WHERE c.id = :id AND c.status = :status AND c.removed = false")
    Optional<CarEntity> findAvailableForSaleById(@Param("id") UUID id, @Param("status") CarStatus status);
}