package ru.carshow.mapper;

import org.springframework.stereotype.Component;
import ru.carshow.dto.*;
import ru.carshow.entity.*;

@Component
public class OrderMapper {

    public Order toResponse(OrderEntity entity) {
        if (entity == null) {
            return null;
        }
        Order dto = new Order();
        dto.setId(entity.getId());
        dto.setClientId(entity.getClient() != null ? entity.getClient().getId() : null);
        dto.setManagerId(entity.getManager() != null ? entity.getManager().getId() : null);
        dto.setStatus(entity.getStatus() != null ? entity.getStatus().name() : null);
        dto.setCreatedAt(entity.getCreatedAt());
        return dto;
    }

    public InStockOrder toResponse(InStockOrderEntity entity) {
        if (entity == null) {
            return null;
        }
        InStockOrder dto = new InStockOrder();
        dto.setId(entity.getId());
        dto.setClientId(entity.getClient() != null ? entity.getClient().getId() : null);
        dto.setManagerId(entity.getManager() != null ? entity.getManager().getId() : null);
        dto.setStatus(entity.getStatus() != null ? entity.getStatus().name() : null);
        dto.setCreatedAt(entity.getCreatedAt());
        dto.setCarId(entity.getCarId());
        return dto;
    }

    public ConfigurationOrder toResponse(ConfigurationOrderEntity entity) {
        if (entity == null) {
            return null;
        }
        ConfigurationOrder dto = new ConfigurationOrder();
        dto.setId(entity.getId());
        dto.setClientId(entity.getClient() != null ? entity.getClient().getId() : null);
        dto.setManagerId(entity.getManager() != null ? entity.getManager().getId() : null);
        dto.setStatus(entity.getStatus() != null ? entity.getStatus().name() : null);
        dto.setCreatedAt(entity.getCreatedAt());
        dto.setCarModelId(entity.getCarModel() != null ? entity.getCarModel().getId() : null);
        dto.setFinalPrice(entity.getFinalPrice() != null ? entity.getFinalPrice().doubleValue() : null);
        return dto;
    }
}
