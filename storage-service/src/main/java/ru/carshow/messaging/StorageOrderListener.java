package ru.carshow.messaging;

import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.stereotype.Component;
import ru.carshow.events.OrderSentForApprovalEvent;
import ru.carshow.events.RabbitNames;
import ru.carshow.service.WarehouseProcessingService;

import java.nio.charset.StandardCharsets;

@Slf4j
@Component
@RequiredArgsConstructor
public class StorageOrderListener {

    private final ObjectMapper objectMapper;
    private final WarehouseProcessingService warehouseProcessingService;

    @RabbitListener(queues = RabbitNames.Q_STORAGE_ORDER_IN)
    public void onMessage(org.springframework.amqp.core.Message message) {
        String body = new String(message.getBody(), StandardCharsets.UTF_8);
        try {
            OrderSentForApprovalEvent evt = objectMapper.readValue(body, OrderSentForApprovalEvent.class);
            warehouseProcessingService.handleOrderSentForApproval(evt);
        } catch (Exception e) {
            log.error("Failed to process OrderSentForApproval", e);
            throw new RuntimeException(e);
        }
    }
}
