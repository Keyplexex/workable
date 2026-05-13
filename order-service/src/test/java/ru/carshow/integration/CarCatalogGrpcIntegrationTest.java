package ru.carshow.integration;

import io.grpc.Server;
import io.grpc.inprocess.InProcessServerBuilder;
import io.grpc.stub.StreamObserver;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.context.annotation.Import;
import org.springframework.http.MediaType;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.DynamicPropertyRegistry;
import org.springframework.test.context.DynamicPropertySource;
import org.springframework.test.web.servlet.MockMvc;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.containers.RabbitMQContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;
import ru.carshow.config.TestJwtDecoderConfig;
import ru.carshow.grpc.inventory.CarIdRequest;
import ru.carshow.grpc.inventory.CarInventoryMessage;
import ru.carshow.grpc.inventory.CarInventoryServiceGrpc;
import ru.carshow.grpc.inventory.CarListResponse;
import ru.carshow.grpc.inventory.CarSingleResponse;
import ru.carshow.grpc.inventory.Empty;

import java.io.IOException;
import java.util.UUID;

import static org.springframework.security.test.web.servlet.request.SecurityMockMvcRequestPostProcessors.jwt;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.jsonPath;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;

@SpringBootTest
@Testcontainers
@AutoConfigureMockMvc
@ActiveProfiles("test")
@Import(TestJwtDecoderConfig.class)
class CarCatalogGrpcIntegrationTest {

    private static final String IN_PROCESS = InProcessServerBuilder.generateName();
    private static final Server grpcServer;

    static {
        try {
            grpcServer = InProcessServerBuilder.forName(IN_PROCESS)
                    .directExecutor()
                    .addService(new CarInventoryServiceGrpc.CarInventoryServiceImplBase() {
                        @Override
                        public void listAvailableCars(Empty request, StreamObserver<CarListResponse> responseObserver) {
                            var car = CarInventoryMessage.newBuilder()
                                    .setId("40000000-0000-0000-0000-000000000001")
                                    .setVin("WBA00000000000001")
                                    .setYear(2023)
                                    .setColor("Black")
                                    .setEnginePower("184")
                                    .setEngineVolume("2.0")
                                    .setFuelType("GASOLINE")
                                    .setTransmission("AUTOMATIC")
                                    .setDriveType("RWD")
                                    .setPrice("3700000")
                                    .setStatus("AVAILABLE")
                                    .setModelId("10000000-0000-0000-0000-000000000001")
                                    .setModelBrand("BMW")
                                    .setModelName("320i")
                                    .build();
                            responseObserver.onNext(CarListResponse.newBuilder().addCars(car).build());
                            responseObserver.onCompleted();
                        }

                        @Override
                        public void getAvailableCar(CarIdRequest request, StreamObserver<CarSingleResponse> responseObserver) {
                            if ("40000000-0000-0000-0000-000000000001".equals(request.getId())) {
                                var car = CarInventoryMessage.newBuilder()
                                        .setId(request.getId())
                                        .setVin("WBA00000000000001")
                                        .setYear(2023)
                                        .setStatus("AVAILABLE")
                                        .setModelId("10000000-0000-0000-0000-000000000001")
                                        .setModelBrand("BMW")
                                        .setModelName("320i")
                                        .build();
                                responseObserver.onNext(CarSingleResponse.newBuilder().setFound(true).setCar(car).build());
                            } else {
                                responseObserver.onNext(CarSingleResponse.newBuilder().setFound(false).build());
                            }
                            responseObserver.onCompleted();
                        }
                    })
                    .build()
                    .start();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
            .withDatabaseName("ord")
            .withUsername("t")
            .withPassword("t");

    @Container
    static RabbitMQContainer rabbit = new RabbitMQContainer("rabbitmq:3.13-alpine");

    @DynamicPropertySource
    static void props(DynamicPropertyRegistry registry) {
        registry.add("spring.datasource.url", postgres::getJdbcUrl);
        registry.add("spring.datasource.username", postgres::getUsername);
        registry.add("spring.datasource.password", postgres::getPassword);
        registry.add("spring.rabbitmq.host", rabbit::getHost);
        registry.add("spring.rabbitmq.port", rabbit::getAmqpPort);
        registry.add("grpc.client.storage.address", () -> "in-process:" + IN_PROCESS);
        registry.add("grpc.client.storage.negotiation-type", () -> "plaintext");
    }

    @AfterAll
    static void stopGrpc() {
        grpcServer.shutdown();
    }

    @Autowired
    private MockMvc mockMvc;

    @Test
    void listCarsReturnsDataFromGrpc() throws Exception {
        mockMvc.perform(get("/api/v1/cars")
                        .with(jwt().authorities(new SimpleGrantedAuthority("ROLE_USER")))
                        .accept(MediaType.APPLICATION_JSON))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$[0].vin").value("WBA00000000000001"));
    }

    @Test
    void getCarReturns404WhenGrpcReportsNotFound() throws Exception {
        UUID random = UUID.randomUUID();
        mockMvc.perform(get("/api/v1/cars/" + random)
                        .with(jwt().authorities(new SimpleGrantedAuthority("ROLE_USER")))
                        .accept(MediaType.APPLICATION_JSON))
                .andExpect(status().isNotFound());
    }
}
