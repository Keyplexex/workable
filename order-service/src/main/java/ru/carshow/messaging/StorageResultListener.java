package ru.carshow.messaging;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.stereotype.Component;
import ru.carshow.events.RabbitNames;
import ru.carshow.service.OrderWarehouseReplyService;

import java.nio.charset.StandardCharsets;

@Slf4j
@Component
@RequiredArgsConstructor
public class StorageResultListener {

    private final OrderWarehouseReplyService orderWarehouseReplyService;

    @RabbitListener(queues = RabbitNames.Q_ORDER_RESULT_IN)
    public void onMessage(org.springframework.amqp.core.Message message) {
        String rk = message.getMessageProperties().getReceivedRoutingKey();
        String body = new String(message.getBody(), StandardCharsets.UTF_8);
        log.debug("Incoming storage reply rk={} body={}", rk, body);
        if (RabbitNames.RK_ORDER_APPROVED.equals(rk)) {
            orderWarehouseReplyService.handleApprovedJson(body);
        } else if (RabbitNames.RK_ORDER_REJECTED.equals(rk)) {
            orderWarehouseReplyService.handleRejectedJson(body);
        } else {
            log.warn("Unknown routing key {}", rk);
        }
    }
}
