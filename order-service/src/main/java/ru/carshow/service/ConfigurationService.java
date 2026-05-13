package ru.carshow.service;

import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.dto.request.BuildConfigurationRequest;
import ru.carshow.dto.response.ConfigurationResult;
import ru.carshow.entity.CarModelEntity;
import ru.carshow.entity.ComponentEntity;
import ru.carshow.exception.DomainValidationException;
import ru.carshow.exception.IncompatibleComponentException;
import ru.carshow.repository.CarModelRepository;
import ru.carshow.repository.ComponentRepository;
import java.math.BigDecimal;
import java.util.*;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
@Transactional(readOnly = true)
public class ConfigurationService {
    
    private final CarModelRepository carModelRepository;
    private final ComponentRepository componentRepository;
    
    public List<ComponentEntity> getDefaultComponents(UUID carModelId) {
        CarModelEntity model = getCarModelOrThrow(carModelId);
        return model.getComponents().stream()
            .filter(ComponentEntity::isDefault)
            .filter(c -> !c.isRemoved())
            .collect(Collectors.toList());
    }
    
    public List<ComponentEntity> getComponentsByType(UUID carModelId, String componentType) {
        CarModelEntity model = getCarModelOrThrow(carModelId);
        return componentRepository.findByCarModelAndType(model, componentType).stream()
            .filter(c -> !c.isRemoved() && c.isCompatibleWith(model))
            .collect(Collectors.toList());
    }
    
    @Transactional
    public ConfigurationResult buildConfiguration(BuildConfigurationRequest request) {
        CarModelEntity model = getCarModelOrThrow(request.getCarModelId());
        Map<String, UUID> selected = request.getSelectedComponents();
        
        Set<String> requiredTypes = model.getComponents().stream()
            .map(ComponentEntity::getType)
            .collect(Collectors.toSet());
        
        for (String type : requiredTypes) {
            if (!selected.containsKey(type)) {
                throw new DomainValidationException(
                    "Отсутствует обязательный узел: " + type);
            }
        }
        
        List<ConfigurationResult.ComponentDto> resultComponents = new ArrayList<>();
        BigDecimal supplementSum = BigDecimal.ZERO;
        
        for (Map.Entry<String, UUID> entry : selected.entrySet()) {
            String type = entry.getKey();
            UUID componentId = entry.getValue();
            
            ComponentEntity component = componentRepository.findById(componentId)
                .filter(c -> !c.isRemoved())
                .orElseThrow(() -> new DomainValidationException(
                    "Компонент не найден: " + componentId));
            
            if (!component.isCompatibleWith(model)) {
                throw new IncompatibleComponentException(
                    String.format("Компонент '%s' не совместим с моделью %s %s",
                        component.getName(), model.getBrand(), model.getName()));
            }
            
            resultComponents.add(new ConfigurationResult.ComponentDto(
                component.getId(), component.getType(), component.getName(), component.getPriceSupplement()));
            
            supplementSum = supplementSum.add(component.getPriceSupplement());
        }
        
        BigDecimal finalPrice = model.getBasePrice().add(supplementSum);
        
        return new ConfigurationResult(
            model.getId(),
            model.getBrand() + " " + model.getName(),
            model.getBasePrice(),
            resultComponents,
            supplementSum,
            finalPrice
        );
    }
    
    private CarModelEntity getCarModelOrThrow(UUID id) {
        return carModelRepository.findById(id)
            .filter(m -> !m.isRemoved())
            .orElseThrow(() -> new DomainValidationException("Модель не найдена: " + id));
    }
}