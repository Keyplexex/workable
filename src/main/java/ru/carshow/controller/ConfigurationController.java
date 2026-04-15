package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import ru.carshow.dto.CarResponse;
import ru.carshow.dto.request.BuildConfigurationRequest;
import ru.carshow.dto.response.ConfigurationResult;
import ru.carshow.entity.ComponentEntity;
import ru.carshow.repository.CarRepository;
import ru.carshow.service.CarService;
import ru.carshow.service.ConfigurationService;
import java.util.List;
import java.util.UUID;

@RestController
@RequestMapping("/api/configurator")
@RequiredArgsConstructor
@Tag(name = "Configurator", description = "Конфигуратор автомобилей")
public class ConfigurationController {
    
    private final ConfigurationService configurationService;
    private final CarService carService;

    @GetMapping("/models/{modelId}/default")
    @Operation(summary = "Получить базовую комплектацию модели")
    public List<ComponentEntity> getDefaultComponents(@PathVariable UUID modelId) {
        return configurationService.getDefaultComponents(modelId);
    }
    
    @GetMapping("/models/{modelId}/components/{type}")
    @Operation(summary = "Получить доступные варианты узла")
    public List<ComponentEntity> getComponentsByType(
            @PathVariable UUID modelId,
            @PathVariable String type) {
        return configurationService.getComponentsByType(modelId, type);
    }
    
    @PostMapping("/build")
    @Operation(summary = "Собрать конфигурацию и рассчитать стоимость")
    public ResponseEntity<ConfigurationResult> buildConfiguration(
            @Valid @RequestBody BuildConfigurationRequest request) {
        return ResponseEntity.ok(configurationService.buildConfiguration(request));
    }

    @DeleteMapping("/cars/{carId}")
    @Operation(summary = "Удалить автомобиль (мягкое удаление)")
    public ResponseEntity<Void> deleteCar(@PathVariable UUID carId) {
        carService.deleteByID(carId);
        return ResponseEntity.noContent().build();
    }
}