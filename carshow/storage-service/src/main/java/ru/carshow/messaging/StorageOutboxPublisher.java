package ru.carshow.messaging;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.amqp.core.Message;
import org.springframework.amqp.core.MessageProperties;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.entity.OutboxMessageEntity;
import ru.carshow.events.RabbitNames;
import ru.carshow.repository.OutboxMessageRepository;

import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.util.List;

@Slf4j
@Component
@RequiredArgsConstructor
public class StorageOutboxPublisher {

    private final OutboxMessageRepository outboxMessageRepository;
    private final RabbitTemplate rabbitTemplate;

    @Scheduled(fixedDelayString = "${storage.outbox.publish-interval-ms:1000}")
    @Transactional
    public void publishPending() {
        List<OutboxMessageEntity> batch = outboxMessageRepository.findTop50ByPublishedAtIsNullOrderByCreatedAtAsc();
        for (OutboxMessageEntity row : batch) {
            try {
                MessageProperties props = new MessageProperties();
                props.setContentType("application/json");
                Message message = new Message(row.getPayload().getBytes(StandardCharsets.UTF_8), props);
                rabbitTemplate.send(RabbitNames.EXCHANGE, row.getRoutingKey(), message);
                row.setPublishedAt(Instant.now());
                outboxMessageRepository.save(row);
                log.info("Published storage outbox {} rk={}", row.getId(), row.getRoutingKey());
            } catch (Exception e) {
                log.error("Failed storage outbox {}", row.getId(), e);
            }
        }
    }
}
