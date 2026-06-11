package ru.carshow.repository;

import org.springframework.data.jpa.repository.JpaSpecificationExecutor;
import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Repository;

import ru.carshow.entity.PartEntity;
import ru.carshow.entity.PartCategory;
import java.util.List;
import java.util.UUID;

public interface PartRepository extends CrudRepository<PartEntity, UUID>, JpaSpecificationExecutor<PartEntity> {
    List<PartEntity> findByCategory(PartCategory category);
    List<PartEntity> findByCompatibleCarModelsId(UUID carModelId);
}