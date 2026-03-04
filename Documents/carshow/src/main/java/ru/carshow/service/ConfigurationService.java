package ru.carshow.service;

import ru.carshow.domain.model.car.CarModel;
import ru.carshow.domain.model.configuration.CarConfiguration;
import ru.carshow.domain.model.configuration.Component;
import ru.carshow.domain.model.configuration.ComponentType;
import ru.carshow.domain.model.configuration.ConfigurationResult;
import ru.carshow.repository.CarModelRepository;
import ru.carshow.repository.ComponentRepository;
import ru.carshow.domain.exception.DomainValidationException;
import ru.carshow.domain.exception.EntityNotFoundException;
import ru.carshow.domain.exception.IncompatibleComponentException;

import java.math.BigDecimal;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

public class ConfigurationService {
    private final CarModelRepository carModelRepository;
    private final ComponentRepository componentRepository;
    
    public ConfigurationService(CarModelRepository carModelRepository, 
                               ComponentRepository componentRepository) {
        this.carModelRepository = carModelRepository;
        this.componentRepository = componentRepository;
    }
    
    public CarConfiguration createConfiguration(Long carModelId) {
        CarModel carModel = carModelRepository.getById(carModelId);
        CarConfiguration configuration = new CarConfiguration(carModel);
        
        List<Component> defaultComponents = componentRepository.findDefaultForCarModel(carModelId);
        for (Component component : defaultComponents) {
            configuration.addComponent(component);
        }
        
        return configuration;
    }
    
    public ConfigurationResult buildConfiguration(Long carModelId, 
                                                  Map<ComponentType, Long> selectedComponentIds) {
        CarModel carModel = carModelRepository.getById(carModelId);
        CarConfiguration configuration = new CarConfiguration(carModel);
        
        for (Map.Entry<ComponentType, Long> entry : selectedComponentIds.entrySet()) {
            Component component = componentRepository.getById(entry.getValue());
            
            if (component.getType() != entry.getKey()) {
                throw new DomainValidationException(
                    String.format("Компонент '%s' не соответствует типу '%s'", 
                        component.getName(), entry.getKey())
                );
            }
            
            try {
                configuration.addComponent(component);
            } catch (IncompatibleComponentException e) {
                return new ConfigurationResult(
                    configuration, 
                    BigDecimal.ZERO, 
                    false, 
                    configuration.getSelectedComponents()
                );
            }
        }
        
        try {
            configuration.validate();
        } catch (DomainValidationException e) {
            return new ConfigurationResult(
                configuration, 
                BigDecimal.ZERO, 
                false, 
                configuration.getSelectedComponents()
            );
        }
        
        BigDecimal totalPrice = configuration.calculateTotalPrice();
        return new ConfigurationResult(
            configuration, 
            totalPrice, 
            true, 
            configuration.getSelectedComponents()
        );
    }
    
    public List<Component> getAvailableComponentsForCarModel(Long carModelId, ComponentType type) {
        carModelRepository.getById(carModelId); 
        
        return componentRepository.findCompatibleWithCarModel(carModelId).stream()
            .filter(component -> component.getType() == type)
            .collect(Collectors.toList());
    }
    
    public Map<ComponentType, List<Component>> getAllAvailableComponents(Long carModelId) {
        carModelRepository.getById(carModelId);
        
        List<Component> compatibleComponents = componentRepository.findCompatibleWithCarModel(carModelId);
        
        return compatibleComponents.stream()
            .collect(Collectors.groupingBy(Component::getType));
    }
    
    public Component createComponent(String name, ComponentType type, String description,
                                     BigDecimal surcharge, Long partId, boolean isDefault) {
        throw new UnsupportedOperationException("Метод временно не реализован");
    }
}