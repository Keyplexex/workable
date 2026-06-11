package ru.carshow.integration;

import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.DynamicPropertyRegistry;
import org.springframework.test.context.DynamicPropertySource;
import org.testcontainers.containers.PostgreSQLContainer;
import org.testcontainers.containers.RabbitMQContainer;
import org.testcontainers.junit.jupiter.Container;
import org.testcontainers.junit.jupiter.Testcontainers;
import ru.carshow.events.OrderSentForApprovalEvent;
import ru.carshow.repository.AssemblyOrderRepository;
import ru.carshow.repository.CarRepository;
import ru.carshow.service.WarehouseProcessingService;

import java.util.List;
import java.util.UUID;

import static org.assertj.core.api.Assertions.assertThat;

@SpringBootTest
@Testcontainers
@ActiveProfiles("test")
class WarehouseProcessingIntegrationTest {

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
    }

    @Autowired
    private WarehouseProcessingService warehouseProcessingService;

    @Autowired
    private AssemblyOrderRepository assemblyOrderRepository;

    @Autowired
    private CarRepository carRepository;

    void reservesCarForPaidInStockOrder() {
        UUID carId = UUID.fromString("40000000-0000-0000-0000-000000000001");
        UUID orderId = UUID.fromString("50000000-0000-0000-0000-000000000001");
        UUID eventId = UUID.fromString("60000000-0000-0000-0000-000000000001");
        UUID traceId = UUID.fromString("70000000-0000-0000-0000-000000000001");

        OrderSentForApprovalEvent evt = new OrderSentForApprovalEvent(
                eventId, traceId, orderId, "INSTOCK", carId, null, List.of());

        warehouseProcessingService.handleOrderSentForApproval(evt);

        assertThat(assemblyOrderRepository.findAll()).anyMatch(a ->
                a.getSourceOrderId().equals(orderId) && "ASSEMBLED".equals(a.getStatus()));
        assertThat(carRepository.findById(carId)).hasValueSatisfying(c ->
                assertThat(c.getStatus().name()).isEqualTo("RESERVED"));
    }
}
