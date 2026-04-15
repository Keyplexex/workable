package ru.carshow.repository;

import org.springframework.data.jpa.repository.JpaSpecificationExecutor;
import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.OrderEntity;
import java.util.List;
import java.util.UUID;

public interface OrderRepository extends CrudRepository<OrderEntity, UUID>, JpaSpecificationExecutor<OrderEntity> {
    List<OrderEntity> findByClientId(UUID clientId);
    List<OrderEntity> findByManagerId(UUID managerId);
}