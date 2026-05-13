package ru.carshow.exception;

import org.springframework.http.ResponseEntity;
import org.springframework.security.access.AccessDeniedException; 
import org.springframework.web.bind.annotation.*;

import java.time.Instant;
import java.util.LinkedHashMap;
import java.util.Map;

@RestControllerAdvice
public class GlobalExceptionHandler {

    @ExceptionHandler(AccessDeniedException.class)
    public ResponseEntity<Map<String, Object>> handleAccessDenied(AccessDeniedException ex) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("timestamp", Instant.now());
        body.put("status", 403); 
        body.put("error", "Forbidden");
        body.put("message", "You do not have permission to perform this action");
        return ResponseEntity.status(403).body(body);
    }

    @ExceptionHandler(DomainValidationException.class)
    public ResponseEntity<Map<String, Object>> handleDomainError(DomainValidationException ex) {
        Map<String, Object> body = new LinkedHashMap<>();
        body.put("timestamp", Instant.now());
        body.put("status", 400);
        body.put("error", "Bad Request");
        body.put("message", ex.getMessage());
        return ResponseEntity.badRequest().body(body);
    }

    @ExceptionHandler(RuntimeException.class)
    public ResponseEntity<Map<String, Object>> handleRuntimeError(RuntimeException ex) {
        System.err.println("🔥 UNHANDLED RUNTIME EXCEPTION: " + ex.getClass().getName() + " - " + ex.getMessage());
        ex.printStackTrace();

        Map<String, Object> body = new LinkedHashMap<>();
        body.put("timestamp", Instant.now());
        body.put("status", 500); 
        body.put("error", "Internal Server Error");
        body.put("message", "An unexpected error occurred: " + ex.getMessage());
        return ResponseEntity.status(500).body(body);
    }
}