package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;
import ru.carshow.dto.TestDriveRequest;
import ru.carshow.entity.TestDriveStatus;
import ru.carshow.security.CurrentUser;
import ru.carshow.service.TestDriveService;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

@RestController
@RequestMapping("/api/test-drives")
@RequiredArgsConstructor
@Tag(name = "Test drives", description = "Заявки на тест-драйв")
public class TestDriveController {

    private final TestDriveService testDriveService;
    private final CurrentUser currentUser;

    @PostMapping
    @PreAuthorize("hasRole('USER')")
    @Operation(summary = "Создать заявку на тест-драйв")
    public ResponseEntity<TestDriveRequest> create(
            @RequestParam UUID carId,
            @RequestParam LocalDateTime startDateTime) {
        return ResponseEntity.ok(testDriveService.createRequest(currentUser.getId(), carId, startDateTime));
    }

    @GetMapping
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'ADMIN')")
    @Operation(summary = "Список заявок")
    public List<TestDriveRequest> list() {
        if (currentUser.hasRole("ADMIN") || currentUser.hasRole("MANAGER")) {
            return testDriveService.getAllRequests();
        }
        return testDriveService.getRequestsByClient(currentUser.getId());
    }

    @GetMapping("/{id}")
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'ADMIN')")
    @Operation(summary = "Заявка по ID")
    public ResponseEntity<TestDriveRequest> get(@PathVariable UUID id) {
        return ResponseEntity.ok(testDriveService.getRequestById(id));
    }

    @PatchMapping("/{id}/confirm")
    @PreAuthorize("hasAnyRole('MANAGER', 'ADMIN')")
    @Operation(summary = "Подтвердить заявку")
    public ResponseEntity<TestDriveRequest> confirm(@PathVariable UUID id) {
        return ResponseEntity.ok(testDriveService.confirmRequest(id));
    }

    @PatchMapping("/{id}/complete")
    @PreAuthorize("hasAnyRole('MANAGER', 'ADMIN')")
    @Operation(summary = "Завершить заявку")
    public ResponseEntity<TestDriveRequest> complete(@PathVariable UUID id) {
        return ResponseEntity.ok(testDriveService.completeRequest(id));
    }

    @PatchMapping("/{id}/cancel")
    @PreAuthorize("hasAnyRole('USER', 'MANAGER', 'ADMIN')")
    @Operation(summary = "Отменить заявку")
    public ResponseEntity<TestDriveRequest> cancel(@PathVariable UUID id) {
        return ResponseEntity.ok(testDriveService.cancelRequest(id));
    }
}
