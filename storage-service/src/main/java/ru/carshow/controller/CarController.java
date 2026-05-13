package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import ru.carshow.dto.CarResponse;
import ru.carshow.dto.CreateCarRequest;
import ru.carshow.service.CarService;

import java.util.List;
import java.util.UUID;

@RestController
@RequestMapping("/api/cars")
@RequiredArgsConstructor
@Tag(name = "Cars", description = "Управление автомобилями в наличии")
public class CarController {

    private final CarService carService;

    @GetMapping
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Получить список доступных автомобилей")
    public List<CarResponse> getAllCars() {
        return carService.getAllCars();
    }

    @GetMapping("/{id}")
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Получить автомобиль по ID")
    public ResponseEntity<CarResponse> getCarById(@PathVariable UUID id) {
        return ResponseEntity.ok(carService.getCarById(id));
    }

    @PostMapping
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Добавить новый автомобиль в салон")
    public ResponseEntity<CarResponse> createCar(@Valid @RequestBody CreateCarRequest request) {
        return ResponseEntity.ok(carService.createCar(request));
    }

    @DeleteMapping("/{id}")
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Удалить автомобиль")
    public ResponseEntity<Void> deleteCar(@PathVariable UUID id) {
        carService.deleteByID(id);
        return ResponseEntity.noContent().build();
    }
}