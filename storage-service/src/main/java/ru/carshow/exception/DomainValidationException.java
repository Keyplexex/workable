package ru.carshow.exception;

public class DomainValidationException extends RuntimeException {
    public DomainValidationException(String message) {
        super(message);
    }
}