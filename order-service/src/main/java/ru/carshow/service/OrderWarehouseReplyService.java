package ru.carshow.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.slf4j.MDC;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.entity.*;
import ru.carshow.events.OrderApprovedEvent;
import ru.carshow.events.OrderRejectedEvent;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.repository.OrderRepository;
import ru.carshow.repository.ProcessedIncomingEventRepository;

import java.time.Instant;
import java.util.UUID;

@Slf4j
@Service
@RequiredArgsConstructor
public class OrderWarehouseReplyService {

    private final OrderRepository orderRepository;
    private final ProcessedIncomingEventRepository processedIncomingEventRepository;
    private final ObjectMapper objectMapper;

    @Transactional
    public void handleApprovedJson(String json) {
        try {
            OrderApprovedEvent evt = objectMapper.readValue(json, OrderApprovedEvent.class);
            applyMdc(evt.traceId(), evt.orderId());
            if (processedIncomingEventRepository.existsById(evt.eventId())) {
                log.debug("Duplicate approved event {}", evt.eventId());
                return;
            }
            OrderEntity order = orderRepository.findById(evt.orderId())
                    .filter(o -> !o.isRemoved())
                    .orElseThrow(() -> new DomainValidationException("Order not found: " + evt.orderId()));
            order.setStatus(OrderStatus.READY_FOR_PICKUP);
            orderRepository.save(order);
            processedIncomingEventRepository.save(processed(evt.eventId()));
            log.info("Order {} marked READY_FOR_PICKUP (assembly {})", evt.orderId(), evt.assemblyOrderId());
        } catch (Exception e) {
            throw new RuntimeException(e);
        } finally {
            MDC.clear();
        }
    }

    @Transactional
    public void handleRejectedJson(String json) {
        try {
            OrderRejectedEvent evt = objectMapper.readValue(json, OrderRejectedEvent.class);
            applyMdc(evt.traceId(), evt.orderId());
            if (processedIncomingEventRepository.existsById(evt.eventId())) {
                log.debug("Duplicate rejected event {}", evt.eventId());
                return;
            }
            OrderEntity order = orderRepository.findById(evt.orderId())
                    .filter(o -> !o.isRemoved())
                    .orElseThrow(() -> new DomainValidationException("Order not found: " + evt.orderId()));
            order.setStatus(OrderStatus.CANCELLED);
            orderRepository.save(order);
            processedIncomingEventRepository.save(processed(evt.eventId()));
            log.warn("Order {} cancelled by warehouse: {}", evt.orderId(), evt.reason());
        } catch (Exception e) {
            throw new RuntimeException(e);
        } finally {
            MDC.clear();
        }
    }

    private static void applyMdc(UUID traceId, UUID orderId) {
        MDC.put("traceId", traceId != null ? traceId.toString() : "");
        MDC.put("orderId", orderId != null ? orderId.toString() : "");
    }

    private static ProcessedIncomingEventEntity processed(UUID eventId) {
        ProcessedIncomingEventEntity e = new ProcessedIncomingEventEntity();
        e.setEventId(eventId);
        e.setProcessedAt(Instant.now());
        return e;
    }
}
