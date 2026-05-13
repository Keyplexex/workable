package ru.carshow.mapper;

import org.springframework.stereotype.Component;
import ru.carshow.dto.TestDriveRequest;
import ru.carshow.entity.TestDriveRequestEntity;

@Component
public class TestDriveMapper {

    public TestDriveRequest toResponse(TestDriveRequestEntity entity) {
        if (entity == null) {
            return null;
        }
        TestDriveRequest dto = new TestDriveRequest();
        dto.setId(entity.getId());
        dto.setClientId(entity.getClient() != null ? entity.getClient().getId() : null);
        dto.setCarId(entity.getCarId());
        dto.setStartDateTime(entity.getStartDateTime());
        dto.setStatus(entity.getStatus() != null ? entity.getStatus().name() : null);
        return dto;
    }
}
