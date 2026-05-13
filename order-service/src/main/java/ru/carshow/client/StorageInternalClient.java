package ru.carshow.client;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.web.client.RestTemplateBuilder;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestClientException;
import org.springframework.web.client.RestTemplate;

import java.time.Duration;
import java.util.UUID;

/**
 * Проверка автомобиля на складе (внутренний вызов без JWT пользователя).
 */
@Slf4j
@Component
public class StorageInternalClient {

    private final RestTemplate restTemplate;
    private final String baseUrl;
    private final String serviceToken;

    public StorageInternalClient(
            RestTemplateBuilder builder,
            @Value("${storage.base-url:}") String baseUrl,
            @Value("${storage.service-token:}") String serviceToken) {
        this.restTemplate = builder
                .setConnectTimeout(Duration.ofSeconds(2))
                .setReadTimeout(Duration.ofSeconds(5))
                .build();
        this.baseUrl = baseUrl == null ? "" : baseUrl.trim();
        this.serviceToken = serviceToken == null ? "" : serviceToken;
    }

    public boolean isCarAvailableForClientOperations(UUID carId) {
        if (baseUrl.isEmpty()) {
            log.warn("storage.base-url is empty; skip remote car validation");
            return true;
        }
        try {
            HttpHeaders headers = new HttpHeaders();
            if (!serviceToken.isEmpty()) {
                headers.set("X-Service-Token", serviceToken);
            }
            HttpEntity<Void> entity = new HttpEntity<>(headers);
            ResponseEntity<CarAvailabilityDto> response = restTemplate.exchange(
                    baseUrl + "/api/internal/cars/" + carId + "/availability",
                    HttpMethod.GET,
                    entity,
                    CarAvailabilityDto.class);
            CarAvailabilityDto body = response.getBody();
            return body != null && body.available() && "AVAILABLE".equalsIgnoreCase(body.status());
        } catch (RestClientException ex) {
            log.error("Storage availability check failed for car {}", carId, ex);
            return false;
        }
    }

    public record CarAvailabilityDto(boolean available, String status) {}
}
