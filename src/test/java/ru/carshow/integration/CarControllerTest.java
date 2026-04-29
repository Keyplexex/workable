package ru.carshow.integration;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.http.MediaType;
import org.springframework.security.test.context.support.WithMockUser; 
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.DynamicPropertyRegistry;
import org.springframework.test.context.DynamicPropertySource;
import org.springframework.test.web.servlet.MockMvc;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;

import ru.carshow.dto.CreateCarRequest;
import ru.carshow.entity.CarModelEntity;
import ru.carshow.entity.FuelType;
import ru.carshow.entity.TransmissionType;
import ru.carshow.entity.DriveType;
import ru.carshow.repository.CarModelRepository;

import java.math.BigDecimal;
import java.util.UUID;

import static org.junit.Assert.assertThat;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@SpringBootTest
@AutoConfigureMockMvc
@Testcontainers
@ActiveProfiles("test")
class CarControllerTest {

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
            .withDatabaseName("testdb")
            .withUsername("test")
            .withPassword("test");

    @DynamicPropertySource
    static void properties(DynamicPropertyRegistry registry) {
        registry.add("spring.datasource.url", postgres::getJdbcUrl);
        registry.add("spring.datasource.username", postgres::getUsername);
        registry.add("spring.datasource.password", postgres::getPassword);
        registry.add("spring.liquibase.enabled", () -> false); 
    }

    @Autowired
    private MockMvc mockMvc;

    @Autowired
    private ObjectMapper objectMapper;

    @Autowired
    private CarModelRepository carModelRepository;

    private UUID testModelId;

    @BeforeEach
    void setUp() {
        CarModelEntity model = new CarModelEntity();
        model.setBrand("BMW");
        model.setName("320i");
        model.setBodyType("SEDAN");
        model.setBasePrice(new BigDecimal("3500000"));
        testModelId = carModelRepository.save(model).getId();
    }

    @Test
    @WithMockUser(username = "client1", roles = {"USER"}) 
    void shouldGetAvailableCars() throws Exception {
        mockMvc.perform(get("/api/cars"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$").isArray());
    }

    @Test
    @WithMockUser(username = "admin1", roles = {"WAREHOUSE_ADMIN", "ADMIN"}) 
    void shouldCreateCar() throws Exception {
        CreateCarRequest request = new CreateCarRequest();
        request.setModelId(testModelId);
        request.setVin("WBA12345678901234");
        request.setYear(2023);
        request.setColor("Черный");
        request.setEnginePower(BigDecimal.valueOf(184.0));
        request.setEngineVolume(BigDecimal.valueOf(2.0));
        request.setFuelType(FuelType.GASOLINE.name());
        request.setTransmission(TransmissionType.AUTOMATIC.name());
        request.setDriveType(DriveType.RWD.name());
        request.setPrice(new BigDecimal("3700000"));

        mockMvc.perform(post("/api/cars")
                .contentType(MediaType.APPLICATION_JSON)
                .content(objectMapper.writeValueAsString(request)))
                .andExpect(status().is2xxSuccessful())
                .andExpect(jsonPath("$.vin").value("WBA12345678901234"));
    }
}