package ru.carshow.repository;

import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.ProcessedIncomingEventEntity;

import java.util.UUID;

public interface ProcessedIncomingEventRepository extends CrudRepository<ProcessedIncomingEventEntity, UUID> {}
