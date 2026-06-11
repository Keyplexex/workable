package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import ru.carshow.dto.*;
import ru.carshow.entity.OrderStatus;
import ru.carshow.service.OrderService;

import java.util.List;
import java.util.Map;
import java.util.UUID;

@RestController
@RequestMapping("/api/orders")
@RequiredArgsConstructor
@Tag(name = "Orders", description = "Управление заказами")
public class OrderController {

    private final OrderService orderService;

    @PostMapping("/in-stock")
    @PreAuthorize("hasRole('USER')")
    @Operation(summary = "Создать заказ на автомобиль в наличии")
    public ResponseEntity<InStockOrder> createInStockOrder(@RequestParam UUID carId) {
        return ResponseEntity.ok(orderService.createInStockOrder(carId));
    }

    @PostMapping("/configuration")
    @PreAuthorize("hasRole('USER')")
    @Operation(summary = "Создать заказ на кастомную конфигурацию")
    public ResponseEntity<ConfigurationOrder> createConfigurationOrder(
            @RequestParam UUID carModelId,
            @RequestBody Map<String, UUID> selectedComponents) {
        return ResponseEntity.ok(orderService.createConfigurationOrder(carModelId, selectedComponents));
    }

    @GetMapping
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'ADMIN')")
    @Operation(summary = "Получить список заказов")
    public List<Order> getOrders() {
        return orderService.getAllOrders();
    }

    @GetMapping("/{id}")
    @PreAuthorize("hasAnyRole('MANAGER', 'ADMIN') or @orderSecurity.isOwner(#id)")
    @Operation(summary = "Получить заказ по ID")
    public ResponseEntity<Order> getOrderById(@PathVariable UUID id) {
        return ResponseEntity.ok(orderService.getOrderById(id));
    }

    @DeleteMapping("/{id}")
    @PreAuthorize("hasRole('ADMIN') or @orderSecurity.isOwner(#id)")
    @Operation(summary = "Отменить заказ")
    public ResponseEntity<Void> cancelOrder(@PathVariable UUID id) {
        orderService.cancelOrder(id);
        return ResponseEntity.noContent().build();
    }

    @PatchMapping("/{id}/status")
    @PreAuthorize("hasRole('MANAGER')")
    @Operation(summary = "Изменить статус заказа")
    public ResponseEntity<Order> updateStatus(
            @PathVariable UUID id,
            @RequestParam OrderStatus newStatus) {
        return ResponseEntity.ok(orderService.updateOrderStatus(id, newStatus));
    }
}
