package ru.carshow.repository;

import org.springframework.data.repository.CrudRepository;
import ru.carshow.entity.UserEntity;
import java.util.UUID;

public interface UserRepository extends CrudRepository<UserEntity, UUID> {
}