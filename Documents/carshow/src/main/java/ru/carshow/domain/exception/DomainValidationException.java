package ru.carshow.domain.exception;

public class DomainValidationException extends RuntimeException{
    public DomainValidationException(String message) {
        super(message);
    }
}
