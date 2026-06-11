package ru.carshow.controller;

import lombok.RequiredArgsConstructor;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import ru.carshow.entity.CarEntity;
import ru.carshow.entity.CarStatus;
import ru.carshow.repository.CarRepository;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.UUID;

@RestController
@RequestMapping("/api/internal/cars")
@RequiredArgsConstructor
public class InternalCarController {

    private final CarRepository carRepository;

    @Value("${storage.service-token:}")
    private String expectedToken;

    @GetMapping("/{id}/availability")
    public ResponseEntity<Map<String, Object>> availability(
            @PathVariable UUID id,
            @RequestHeader(value = "X-Service-Token", required = false) String token) {
        if (expectedToken != null && !expectedToken.isEmpty()) {
            if (token == null || !expectedToken.equals(token)) {
                return ResponseEntity.status(401).build();
            }
        }
        CarEntity car = carRepository.findById(id).filter(c -> !c.isRemoved()).orElse(null);
        Map<String, Object> body = new LinkedHashMap<>();
        if (car == null) {
            body.put("available", false);
            body.put("status", "NOT_FOUND");
            return ResponseEntity.ok(body);
        }
        body.put("available", car.getStatus() == CarStatus.AVAILABLE);
        body.put("status", car.getStatus() != null ? car.getStatus().name() : null);
        return ResponseEntity.ok(body);
    }
}
