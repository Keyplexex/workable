package ru.carshow.repository;

import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.AssemblyOrderEntity;

import java.util.List;
import java.util.UUID;

public interface AssemblyOrderRepository extends CrudRepository<AssemblyOrderEntity, UUID> {
    List<AssemblyOrderEntity> findAllByRemovedIsFalseOrderByCreatedAtDesc();
}
