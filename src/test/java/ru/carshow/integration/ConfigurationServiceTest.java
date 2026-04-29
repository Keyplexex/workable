package ru.carshow.integration;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.DynamicPropertyRegistry;
import org.springframework.test.context.DynamicPropertySource;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;

import ru.carshow.dto.request.BuildConfigurationRequest;
import ru.carshow.dto.response.ConfigurationResult;
import ru.carshow.entity.CarModelEntity;
import ru.carshow.entity.ComponentEntity;
import ru.carshow.exception.DomainValidationException; 
import ru.carshow.repository.CarModelRepository;
import ru.carshow.repository.ComponentRepository;
import ru.carshow.service.ConfigurationService;

import java.math.BigDecimal;
import java.util.Collections;
import java.util.Map;
import java.util.UUID;

import static org.junit.jupiter.api.Assertions.*;

@SpringBootTest
@Testcontainers
@ActiveProfiles("test")
class ConfigurationServiceTest {
    
    @Autowired
    private ConfigurationService configurationService;
    
    @Autowired
    private CarModelRepository carModelRepository;
    
    @Autowired
    private ComponentRepository componentRepository;

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
    
    @Test
    void buildConfiguration_validSelection_shouldCalculatePrice() {
        CarModelEntity model = new CarModelEntity();
        model.setBrand("TestBrand");
        model.setName("TestModel");
        model.setBasePrice(new BigDecimal("1000000"));
        CarModelEntity savedModel = carModelRepository.save(model);
        
        BuildConfigurationRequest request = new BuildConfigurationRequest();
        request.setCarModelId(savedModel.getId());
        request.setSelectedComponents(Collections.emptyMap());
        
        assertDoesNotThrow(() -> {
            configurationService.buildConfiguration(request);
        });
    }

    @Test
    void buildConfiguration_incompatibleComponent_shouldThrowException() {
        CarModelEntity model = new CarModelEntity();
        model.setBrand("TestBrand");
        model.setName("TestModel");
        model.setBasePrice(new BigDecimal("1000000"));
        CarModelEntity savedModel = carModelRepository.save(model);
        
        ComponentEntity compatible = new ComponentEntity();
        compatible.setName("Compatible");
        compatible.setType("engine");
        compatible.setPriceSupplement(new BigDecimal("10000"));
        compatible.setCarModel(savedModel);
        compatible.setDefault(true);
        compatible.setRemoved(false);
        componentRepository.save(compatible);
        
        CarModelEntity other = new CarModelEntity();
        other.setBrand("Other");
        other.setName("OtherModel");
        other.setBasePrice(new BigDecimal("500000"));
        CarModelEntity savedOther = carModelRepository.save(other);
        
        ComponentEntity incompatible = new ComponentEntity();
        incompatible.setName("Incompatible");
        incompatible.setType("interior");
        incompatible.setPriceSupplement(new BigDecimal("20000"));
        incompatible.setCarModel(savedOther); 
        incompatible.setDefault(false);
        incompatible.setRemoved(false);
        componentRepository.save(incompatible);
        
        BuildConfigurationRequest request = new BuildConfigurationRequest();
        request.setCarModelId(savedModel.getId());
        request.setSelectedComponents(Map.of(
            "interior", incompatible.getId()
        ));
        
        assertThrows(DomainValidationException.class, () -> 
            configurationService.buildConfiguration(request));
    }
}