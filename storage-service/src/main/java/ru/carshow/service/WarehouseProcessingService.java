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
import ru.carshow.events.OrderSentForApprovalEvent;
import ru.carshow.events.RabbitNames;
import ru.carshow.repository.*;

import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

@Slf4j
@Service
@RequiredArgsConstructor
public class WarehouseProcessingService {

    public static final String STATUS_CREATED = "CREATED";
    public static final String STATUS_ASSEMBLED = "ASSEMBLED";
    public static final String STATUS_FAIL = "FAIL";

    private final ProcessedIncomingEventRepository processedIncomingEventRepository;
    private final AssemblyOrderRepository assemblyOrderRepository;
    private final CarRepository carRepository;
    private final PartRepository partRepository;
    private final OutboxMessageRepository outboxMessageRepository;
    private final ObjectMapper objectMapper;

    @Transactional
    public void handleOrderSentForApproval(OrderSentForApprovalEvent evt) {
        if (processedIncomingEventRepository.existsById(evt.eventId())) {
            log.debug("Skip duplicate warehouse event {}", evt.eventId());
            return;
        }
        MDC.put("traceId", evt.traceId() != null ? evt.traceId().toString() : "");
        MDC.put("orderId", evt.orderId() != null ? evt.orderId().toString() : "");
        try {
            UUID assemblyId = UUID.randomUUID();
            AssemblyOrderEntity assembly = new AssemblyOrderEntity();
            assembly.setId(assemblyId);
            assembly.setSourceOrderId(evt.orderId());
            assembly.setSourceOrderType(evt.orderType());
            assembly.setCarId(evt.carId());
            assembly.setCarModelId(evt.carModelId());
            assembly.setStatus(STATUS_CREATED);
            assembly.setRequiredPartsJson(writeJson(evt.selectedComponentIds()));

            String failReason = null;
            try {
                if ("INSTOCK".equalsIgnoreCase(evt.orderType())) {
                    failReason = processInStock(evt);
                } else if ("CONFIGURATION".equalsIgnoreCase(evt.orderType())) {
                    failReason = processConfiguration(evt);
                } else {
                    failReason = "Unknown order type: " + evt.orderType();
                }

                if (failReason != null) {
                    assembly.setStatus(STATUS_FAIL);
                    assemblyOrderRepository.save(assembly);
                    enqueueRejected(evt, assemblyId, failReason);
                } else {
                    assembly.setStatus(STATUS_ASSEMBLED);
                    assemblyOrderRepository.save(assembly);
                    enqueueApproved(evt, assemblyId);
                }
            } catch (Exception ex) {
                log.error("Warehouse processing failed", ex);
                assembly.setStatus(STATUS_FAIL);
                assemblyOrderRepository.save(assembly);
                enqueueRejected(evt, assemblyId, ex.getMessage());
            }

            ProcessedIncomingEventEntity done = new ProcessedIncomingEventEntity();
            done.setEventId(evt.eventId());
            done.setProcessedAt(Instant.now());
            processedIncomingEventRepository.save(done);
        } finally {
            MDC.clear();
        }
    }

    private String processInStock(OrderSentForApprovalEvent evt) {
        if (evt.carId() == null) {
            return "Missing carId for in-stock order";
        }
        CarEntity car = carRepository.findById(evt.carId()).filter(c -> !c.isRemoved()).orElse(null);
        if (car == null) {
            return "Car not found";
        }
        if (car.getStatus() != CarStatus.AVAILABLE) {
            return "Car not available, status=" + car.getStatus();
        }
        car.setStatus(CarStatus.RESERVED);
        carRepository.save(car);
        return null;
    }

    private String processConfiguration(OrderSentForApprovalEvent evt) {
        List<UUID> ids = evt.selectedComponentIds() == null ? List.of() : evt.selectedComponentIds();
        if (ids.isEmpty()) {
            return "No components selected for configuration order";
        }
        List<PartEntity> parts = new ArrayList<>();
        for (UUID partId : ids) {
            PartEntity p = partRepository.findById(partId).filter(x -> !x.isRemoved()).orElse(null);
            if (p == null) {
                return "Part not found: " + partId;
            }
            if (p.getQuantity() == null || p.getQuantity() < 1) {
                return "Insufficient quantity for part: " + partId;
            }
            parts.add(p);
        }
        for (PartEntity p : parts) {
            p.setQuantity(p.getQuantity() - 1);
            partRepository.save(p);
        }
        return null;
    }

    private void enqueueApproved(OrderSentForApprovalEvent evt, UUID assemblyOrderId) {
        UUID replyId = UUID.randomUUID();
        OrderApprovedEvent reply = new OrderApprovedEvent(replyId, evt.traceId(), evt.orderId(), assemblyOrderId);
        saveOutbox(replyId, evt.orderId(), RabbitNames.RK_ORDER_APPROVED, reply);
    }

    private void enqueueRejected(OrderSentForApprovalEvent evt, UUID assemblyOrderId, String reason) {
        UUID replyId = UUID.randomUUID();
        OrderRejectedEvent reply = new OrderRejectedEvent(replyId, evt.traceId(), evt.orderId(), reason, assemblyOrderId);
        saveOutbox(replyId, evt.orderId(), RabbitNames.RK_ORDER_REJECTED, reply);
    }

    private void saveOutbox(UUID id, UUID aggregateId, String rk, Object payload) {
        OutboxMessageEntity row = new OutboxMessageEntity();
        row.setId(id);
        row.setCreatedAt(Instant.now());
        row.setAggregateType("ASSEMBLY_REPLY");
        row.setAggregateId(aggregateId);
        row.setRoutingKey(rk);
        try {
            row.setPayload(objectMapper.writeValueAsString(payload));
        } catch (Exception e) {
            throw new IllegalStateException(e);
        }
        outboxMessageRepository.save(row);
    }

    private String writeJson(Object o) {
        try {
            return objectMapper.writeValueAsString(o);
        } catch (Exception e) {
            return "[]";
        }
    }
}
