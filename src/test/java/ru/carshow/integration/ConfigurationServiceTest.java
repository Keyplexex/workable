package ru.carshow.integration;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.DynamicPropertyRegistry;
import org.springframework.test.context.DynamicPropertySource;
import org.springframework.transaction.annotation.Transactional;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;
import ru.carshow.dto.request.BuildConfigurationRequest;
import ru.carshow.dto.response.ConfigurationResult;
import ru.carshow.exception.IncompatibleComponentException;
import ru.carshow.service.ConfigurationService;
import java.math.BigDecimal;
import java.util.Map;
import java.util.UUID;
import static org.junit.jupiter.api.Assertions.*;

@SpringBootTest
@Testcontainers
@Transactional
class ConfigurationServiceTest {
    
    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
            .withDatabaseName("carshow_test")
            .withUsername("test")
            .withPassword("test");
    
    @DynamicPropertySource
    static void properties(DynamicPropertyRegistry registry) {
        registry.add("spring.datasource.url", postgres::getJdbcUrl);
        registry.add("spring.datasource.username", postgres::getUsername);
        registry.add("spring.datasource.password", postgres::getPassword);
        registry.add("spring.liquibase.enabled", () -> false);
    }
    
    @Autowired private ConfigurationService configurationService;
    
    @Test
    void buildConfiguration_validSelection_shouldCalculatePrice() {
        UUID modelId = UUID.fromString("00000000-0000-0000-0000-000000000001");
        
        BuildConfigurationRequest request = new BuildConfigurationRequest();
        request.setCarModelId(modelId);
        request.setSelectedComponents(Map.of(
            "wheels", UUID.fromString("00000000-0000-0000-0000-000000000010"), 
            "transmission", UUID.fromString("00000000-0000-0000-0000-000000000020"), 
            "steering", UUID.fromString("00000000-0000-0000-0000-000000000030"), 
            "interior", UUID.fromString("00000000-0000-0000-0000-000000000040") 
        ));
        
        ConfigurationResult result = configurationService.buildConfiguration(request);
        
        assertNotNull(result);
        assertEquals(modelId, result.getCarModelId());
        assertTrue(result.getFinalPrice().compareTo(result.getBasePrice()) > 0);
    }
    
    @Test
    void buildConfiguration_incompatibleComponent_shouldThrowException() {
        UUID modelId = UUID.fromString("00000000-0000-0000-0000-000000000001"); 
        UUID incompatibleInterior = UUID.fromString("00000000-0000-0000-0000-000000000099"); 
        
        BuildConfigurationRequest request = new BuildConfigurationRequest();
        request.setCarModelId(modelId);
        request.setSelectedComponents(Map.of(
            "wheels", UUID.fromString("00000000-0000-0000-0000-000000000010"),
            "transmission", UUID.fromString("00000000-0000-0000-0000-000000000020"),
            "steering", UUID.fromString("00000000-0000-0000-0000-000000000030"),
            "interior", incompatibleInterior 
        ));
        
        assertThrows(IncompatibleComponentException.class, () -> 
            configurationService.buildConfiguration(request));
    }
}