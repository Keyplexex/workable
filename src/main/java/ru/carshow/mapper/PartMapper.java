package ru.carshow.mapper;

import org.springframework.stereotype.Component;
import ru.carshow.dto.Part;
import ru.carshow.entity.PartEntity;

@Component
public class PartMapper {
    
    public Part toResponse(PartEntity entity) {
        if (entity == null) return null;
        Part dto = new Part();
        dto.setId(entity.getId());
        dto.setName(entity.getName());
        dto.setCategory(entity.getCategory() != null ? entity.getCategory().name() : null);
        dto.setPrice(entity.getPrice());
        dto.setManufacturer(entity.getManufacturer());
        dto.setPartNumber(entity.getPartNumber());
        dto.setQuantity(entity.getQuantity());
        return dto;
    }
}