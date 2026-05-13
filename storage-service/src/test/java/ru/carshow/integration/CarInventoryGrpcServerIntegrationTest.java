package ru.carshow.integration;

import io.grpc.Grpc;
import io.grpc.InsecureChannelCredentials;
import io.grpc.ManagedChannel;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.context.annotation.Import;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.DynamicPropertyRegistry;
import org.springframework.test.context.DynamicPropertySource;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.containers.RabbitMQContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;
import ru.carshow.config.TestJwtDecoderConfig;
import ru.carshow.grpc.inventory.CarIdRequest;
import ru.carshow.grpc.inventory.CarInventoryServiceGrpc;
import ru.carshow.grpc.inventory.Empty;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.net.ServerSocket;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

import static org.assertj.core.api.Assertions.assertThat;

@SpringBootTest
@Testcontainers
@ActiveProfiles("test")
@Import(TestJwtDecoderConfig.class)
class CarInventoryGrpcServerIntegrationTest {

    private static final int GRPC_PORT = freePort();

    @Container
    static PostgreSQLContainer<?> postgres = new PostgreSQLContainer<>("postgres:15")
            .withDatabaseName("st")
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
        registry.add("grpc.server.port", () -> GRPC_PORT);
    }

    @Test
    void grpcListsSeedAvailableCar() throws Exception {
        ManagedChannel channel = Grpc.newChannelBuilderForAddress("localhost", GRPC_PORT, InsecureChannelCredentials.create())
                .build();
        try {
            var stub = CarInventoryServiceGrpc.newBlockingStub(channel);
            var resp = stub.listAvailableCars(Empty.getDefaultInstance());
            assertThat(resp.getCarsList()).isNotEmpty();
            assertThat(resp.getCarsList().get(0).getVin()).isEqualTo("WBA00000000000001");
        } finally {
            channel.shutdown();
            channel.awaitTermination(5, TimeUnit.SECONDS);
        }
    }

    @Test
    void grpcGetAvailableReturnsFoundForSeedCar() throws Exception {
        ManagedChannel channel = Grpc.newChannelBuilderForAddress("localhost", GRPC_PORT, InsecureChannelCredentials.create())
                .build();
        try {
            var stub = CarInventoryServiceGrpc.newBlockingStub(channel);
            var r = stub.getAvailableCar(
                    CarIdRequest.newBuilder().setId("40000000-0000-0000-0000-000000000001").build());
            assertThat(r.getFound()).isTrue();
            assertThat(r.getCar().getId()).isEqualTo("40000000-0000-0000-0000-000000000001");
        } finally {
            channel.shutdown();
            channel.awaitTermination(5, TimeUnit.SECONDS);
        }
    }

    @Test
    void grpcGetAvailableReturnsNotFoundForRandomId() throws Exception {
        ManagedChannel channel = Grpc.newChannelBuilderForAddress("localhost", GRPC_PORT, InsecureChannelCredentials.create())
                .build();
        try {
            var stub = CarInventoryServiceGrpc.newBlockingStub(channel);
            var r = stub.getAvailableCar(
                    CarIdRequest.newBuilder().setId(UUID.randomUUID().toString()).build());
            assertThat(r.getFound()).isFalse();
        } finally {
            channel.shutdown();
            channel.awaitTermination(5, TimeUnit.SECONDS);
        }
    }

    private static int freePort() {
        try (ServerSocket socket = new ServerSocket(0)) {
            return socket.getLocalPort();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }
}
