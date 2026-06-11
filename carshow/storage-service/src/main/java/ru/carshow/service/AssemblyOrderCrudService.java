package ru.carshow.service;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.dto.assembly.AssemblyOrderCreateRequest;
import ru.carshow.dto.assembly.AssemblyOrderResponse;
import ru.carshow.dto.assembly.AssemblyOrderUpdateRequest;
import ru.carshow.entity.AssemblyOrderEntity;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.repository.AssemblyOrderRepository;

import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
@Transactional(readOnly = true)
public class AssemblyOrderCrudService {

    private final AssemblyOrderRepository assemblyOrderRepository;
    private final ObjectMapper objectMapper;

    @Transactional
    public AssemblyOrderResponse create(AssemblyOrderCreateRequest req) {
        AssemblyOrderEntity e = new AssemblyOrderEntity();
        e.setSourceOrderId(req.getSourceOrderId());
        e.setSourceOrderType(req.getSourceOrderType());
        e.setCarId(req.getCarId());
        e.setCarModelId(req.getCarModelId());
        e.setWarehouseOperatorId(req.getWarehouseOperatorId());
        e.setStatus(req.getStatus() != null ? req.getStatus() : WarehouseProcessingService.STATUS_CREATED);
        e.setRequiredPartsJson(writeJson(req.getRequiredPartIds()));
        return toDto(assemblyOrderRepository.save(e));
    }

    public List<AssemblyOrderResponse> findAll() {
        return assemblyOrderRepository.findAllByRemovedIsFalseOrderByCreatedAtDesc().stream()
                .map(this::toDto)
                .collect(Collectors.toList());
    }

    public AssemblyOrderResponse getById(UUID id) {
        return assemblyOrderRepository.findById(id)
                .filter(a -> !a.isRemoved())
                .map(this::toDto)
                .orElseThrow(() -> new DomainValidationException("Assembly order not found: " + id));
    }

    @Transactional
    public AssemblyOrderResponse update(UUID id, AssemblyOrderUpdateRequest req) {
        AssemblyOrderEntity e = assemblyOrderRepository.findById(id)
                .filter(a -> !a.isRemoved())
                .orElseThrow(() -> new DomainValidationException("Assembly order not found: " + id));
        if (req.getCarId() != null) {
            e.setCarId(req.getCarId());
        }
        if (req.getCarModelId() != null) {
            e.setCarModelId(req.getCarModelId());
        }
        if (req.getWarehouseOperatorId() != null) {
            e.setWarehouseOperatorId(req.getWarehouseOperatorId());
        }
        if (req.getStatus() != null) {
            e.setStatus(req.getStatus());
        }
        if (req.getRequiredPartIds() != null) {
            e.setRequiredPartsJson(writeJson(req.getRequiredPartIds()));
        }
        return toDto(assemblyOrderRepository.save(e));
    }

    @Transactional
    public void delete(UUID id) {
        AssemblyOrderEntity e = assemblyOrderRepository.findById(id)
                .orElseThrow(() -> new DomainValidationException("Assembly order not found: " + id));
        e.setRemoved(true);
        assemblyOrderRepository.save(e);
    }

    private AssemblyOrderResponse toDto(AssemblyOrderEntity e) {
        AssemblyOrderResponse r = new AssemblyOrderResponse();
        r.setId(e.getId());
        r.setSourceOrderId(e.getSourceOrderId());
        r.setSourceOrderType(e.getSourceOrderType());
        r.setCarId(e.getCarId());
        r.setCarModelId(e.getCarModelId());
        r.setWarehouseOperatorId(e.getWarehouseOperatorId());
        r.setStatus(e.getStatus());
        r.setRemoved(e.isRemoved());
        r.setCreatedAt(e.getCreatedAt());
        r.setUpdatedAt(e.getUpdatedAt());
        r.setRequiredPartIds(readJson(e.getRequiredPartsJson()));
        return r;
    }

    private List<UUID> readJson(String json) {
        if (json == null || json.isBlank()) {
            return List.of();
        }
        try {
            return objectMapper.readValue(json, new TypeReference<>() {});
        } catch (Exception ex) {
            return List.of();
        }
    }

    private String writeJson(List<UUID> ids) {
        try {
            return objectMapper.writeValueAsString(ids == null ? List.of() : ids);
        } catch (Exception e) {
            throw new IllegalStateException(e);
        }
    }
}
