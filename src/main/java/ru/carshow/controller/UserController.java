package ru.carshow.controller;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.tags.Tag;
import lombok.RequiredArgsConstructor;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import ru.carshow.dto.Client;
import ru.carshow.dto.User;
import ru.carshow.service.UserService;

import java.util.List;
import java.util.UUID;

@RestController
@RequestMapping("/api/users")
@RequiredArgsConstructor
@Tag(name = "Users", description = "Управление пользователями системы")
public class UserController {

    private final UserService userService;

    @PostMapping("/clients")
    @Operation(summary = "Зарегистрировать нового клиента")
    public ResponseEntity<Client> createClient(
            @RequestParam String name,
            @RequestParam String email,
            @RequestParam String phone) {
        return ResponseEntity.ok(userService.createClient(name, email, phone));
    }

    @GetMapping("/{id}")
    @Operation(summary = "Получить информацию о пользователе по ID")
    public ResponseEntity<User> getUserById(@PathVariable UUID id) {
        return ResponseEntity.ok(userService.getUserById(id));
    }

    @GetMapping
    @Operation(summary = "Получить список всех пользователей")
    public List<User> getAllUsers() {
        return userService.getAllUsers();
    }

    @DeleteMapping("/{id}")
    @Operation(summary = "Удалить пользователя")
    public ResponseEntity<Void> deleteUser(@PathVariable UUID id) {
        userService.deleteByID(id);
        return ResponseEntity.noContent().build();
    }
}