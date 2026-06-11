package ru.carshow.repository;

import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.CarModelEntity;
import java.util.UUID;

public interface CarModelRepository extends CrudRepository<CarModelEntity, UUID> {
}