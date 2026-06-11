package ru.carshow.repository;

import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.data.repository.query.Param;
import ru.carshow.entity.CarModelEntity;
import ru.carshow.entity.ComponentEntity;
import java.util.List;

public interface ComponentRepository extends CrudRepository<ComponentEntity, java.util.UUID> {
    
    List<ComponentEntity> findByCarModelAndIsDefaultTrue(CarModelEntity carModel);
    
    List<ComponentEntity> findByCarModelAndType(CarModelEntity carModel, String type);
    
    @Query("SELECT c FROM ComponentEntity c JOIN c.compatibleCarModels cm WHERE cm.id = :carModelId AND c.removed = false")
    List<ComponentEntity> findCompatibleWithCarModel(@Param("carModelId") java.util.UUID carModelId);
}