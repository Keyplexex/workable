package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import jakarta.validation.Valid;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import ru.carshow.dto.assembly.AssemblyOrderCreateRequest;
import ru.carshow.dto.assembly.AssemblyOrderResponse;
import ru.carshow.dto.assembly.AssemblyOrderUpdateRequest;
import ru.carshow.service.AssemblyOrderCrudService;

import java.util.List;
import java.util.UUID;

@RestController
@RequestMapping("/api/assembly-orders")
@RequiredArgsConstructor
@Tag(name = "Assembly orders", description = "Заказы на сборку (склад)")
public class AssemblyOrderController {

    private final AssemblyOrderCrudService assemblyOrderCrudService;

    @PostMapping
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Создать заказ на сборку")
    public ResponseEntity<AssemblyOrderResponse> create(@Valid @RequestBody AssemblyOrderCreateRequest request) {
        return ResponseEntity.ok(assemblyOrderCrudService.create(request));
    }

    @GetMapping
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Список заказов на сборку")
    public List<AssemblyOrderResponse> list() {
        return assemblyOrderCrudService.findAll();
    }

    @GetMapping("/{id}")
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Получить заказ на сборку")
    public ResponseEntity<AssemblyOrderResponse> get(@PathVariable UUID id) {
        return ResponseEntity.ok(assemblyOrderCrudService.getById(id));
    }

    @PutMapping("/{id}")
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Обновить заказ на сборку")
    public ResponseEntity<AssemblyOrderResponse> update(
            @PathVariable UUID id,
            @Valid @RequestBody AssemblyOrderUpdateRequest request) {
        return ResponseEntity.ok(assemblyOrderCrudService.update(id, request));
    }

    @DeleteMapping("/{id}")
    @PreAuthorize("hasAnyRole('WAREHOUSE_ADMIN', 'ADMIN')")
    @Operation(summary = "Удалить (пометить) заказ на сборку")
    public ResponseEntity<Void> delete(@PathVariable UUID id) {
        assemblyOrderCrudService.delete(id);
        return ResponseEntity.noContent().build();
    }
}
