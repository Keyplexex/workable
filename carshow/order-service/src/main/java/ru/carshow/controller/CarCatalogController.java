package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.server.ResponseStatusException;
import ru.carshow.client.CarInventoryGrpcClient;
import ru.carshow.dto.CarInventoryDto;
import ru.carshow.grpc.inventory.CarInventoryMessage;

import java.util.List;
import java.util.UUID;

import static org.springframework.http.HttpStatus.NOT_FOUND;

@RestController
@RequestMapping("/api/v1/cars")
@RequiredArgsConstructor
@Tag(name = "Inventory cars", description = "Автомобили в наличии (данные со склада по gRPC)")
public class CarCatalogController {

    private final CarInventoryGrpcClient carInventoryGrpcClient;

    @GetMapping
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'ADMIN')")
    @Operation(summary = "Список автомобилей в наличии")
    public List<CarInventoryDto> listAvailable() {
        return carInventoryGrpcClient.listAvailableCars().stream()
                .map(this::toDto)
                .toList();
    }

    @GetMapping("/{id}")
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'ADMIN')")
    @Operation(summary = "Автомобиль в наличии по идентификатору")
    public ResponseEntity<CarInventoryDto> getAvailable(@PathVariable UUID id) {
        return carInventoryGrpcClient
                .getAvailableCar(id)
                .map(m -> ResponseEntity.ok(toDto(m)))
                .orElseThrow(() -> new ResponseStatusException(NOT_FOUND, "Car not available for sale"));
    }

    private CarInventoryDto toDto(CarInventoryMessage m) {
        return CarInventoryDto.builder()
                .id(UUID.fromString(m.getId()))
                .vin(m.getVin())
                .year(m.getYear())
                .color(emptyToNull(m.getColor()))
                .enginePower(emptyToNull(m.getEnginePower()))
                .engineVolume(emptyToNull(m.getEngineVolume()))
                .fuelType(emptyToNull(m.getFuelType()))
                .transmission(emptyToNull(m.getTransmission()))
                .driveType(emptyToNull(m.getDriveType()))
                .price(emptyToNull(m.getPrice()))
                .status(emptyToNull(m.getStatus()))
                .modelId(m.getModelId().isEmpty() ? null : UUID.fromString(m.getModelId()))
                .modelBrand(emptyToNull(m.getModelBrand()))
                .modelName(emptyToNull(m.getModelName()))
                .build();
    }

    private static String emptyToNull(String s) {
        return s == null || s.isEmpty() ? null : s;
    }
}
