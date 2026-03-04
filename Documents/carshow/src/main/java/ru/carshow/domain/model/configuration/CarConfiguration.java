package ru.carshow.domain.model.configuration;

import ru.carshow.domain.model.car.CarModel;
import ru.carshow.domain.exception.IncompatibleComponentException;
import ru.carshow.domain.exception.DomainValidationException;

import java.math.BigDecimal;
import java.util.HashMap;
import java.util.Map;

public class CarConfiguration {
    private final CarModel carModel;
    private final Map<ComponentType, Component> selectedComponents;
    
    public CarConfiguration(CarModel carModel) {
        this.carModel = carModel;
        this.selectedComponents = new HashMap<>();
    }
    
    public CarModel getCarModel() {
        return carModel;
    }
    
    public Map<ComponentType, Component> getSelectedComponents() {
        return new HashMap<>(selectedComponents);
    }
    
    public void addComponent(Component component) {
        validateComponent(component);
        selectedComponents.put(component.getType(), component);
    }
    
    private void validateComponent(Component component) {
        if (!component.isCompatibleWithCarModel(carModel.getId())) {
            throw new IncompatibleComponentException(
                String.format("Компонент '%s' недоступен для модели %s %s", 
                    component.getName(), carModel.getBrand(), carModel.getName())
            );
        }
    }
    
    public void validate() {
        for (ComponentType type : ComponentType.values()) {
            if (!selectedComponents.containsKey(type)) {
                throw new DomainValidationException(
                    String.format("Отсутствует обязательный узел '%s'", type)
                );
            }
        }
    }
    
    public BigDecimal calculateTotalPrice() {
        BigDecimal total = carModel.getBasePrice();
        for (Component component : selectedComponents.values()) {
            total = total.add(component.getSurcharge());
        }
        return total;
    }
}