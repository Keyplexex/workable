package ru.carshow.repository;

import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.OutboxMessageEntity;

import java.util.List;
import java.util.UUID;

public interface OutboxMessageRepository extends CrudRepository<OutboxMessageEntity, UUID> {
    List<OutboxMessageEntity> findTop50ByPublishedAtIsNullOrderByCreatedAtAsc();
}
