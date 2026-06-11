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
import ru.carshow.grpc.inventory.CarInventoryServiceGrpc;
import ru.carshow.grpc.inventory.CarListResponse;
import ru.carshow.grpc.inventory.CarSingleResponse;
import ru.carshow.grpc.inventory.Empty;

import java.io.IOException;

import static org.springframework.security.test.web.servlet.request.SecurityMockMvcRequestPostProcessors.jwt;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.content;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;

@SpringBootTest
@Testcontainers
@AutoConfigureMockMvc
@ActiveProfiles("test")
@Import(TestJwtDecoderConfig.class)
class CarCatalogGrpcEmptyListIntegrationTest {

    private static final String IN_PROCESS = InProcessServerBuilder.generateName();
    private static final Server grpcServer;

    static {
        try {
            grpcServer = InProcessServerBuilder.forName(IN_PROCESS)
                    .directExecutor()
                    .addService(new CarInventoryServiceGrpc.CarInventoryServiceImplBase() {
                        @Override
                        public void listAvailableCars(Empty request, StreamObserver<CarListResponse> responseObserver) {
                            responseObserver.onNext(CarListResponse.getDefaultInstance());
                            responseObserver.onCompleted();
                        }

                        @Override
                        public void getAvailableCar(CarIdRequest request, StreamObserver<CarSingleResponse> responseObserver) {
                            responseObserver.onNext(CarSingleResponse.newBuilder().setFound(false).build());
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
    void listCarsReturnsEmptyJsonArray() throws Exception {
        mockMvc.perform(get("/api/v1/cars")
                        .with(jwt().authorities(new SimpleGrantedAuthority("ROLE_USER")))
                        .accept(MediaType.APPLICATION_JSON))
                .andExpect(status().isOk())
                .andExpect(content().json("[]"));
    }
}
