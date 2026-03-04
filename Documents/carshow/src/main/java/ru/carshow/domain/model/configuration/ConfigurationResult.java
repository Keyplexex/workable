package ru.carshow.domain.model.configuration;

import java.math.BigDecimal;
import java.util.Map;

import ru.carshow.domain.model.car.Car;
import ru.carshow.domain.model.configuration.CarConfiguration;
import ru.carshow.domain.model.configuration.Component;

public class ConfigurationResult {
    private final CarConfiguration configuration;
    private final BigDecimal totalPrice;
    private final boolean isValid;
    private final Map<ComponentType, Component> selectedComponents;

    public ConfigurationResult(CarConfiguration configuration, BigDecimal totalPrice, 
                               boolean isValid, Map<ComponentType, Component> selectedComponents) {
        this.configuration = configuration;
        this.totalPrice = totalPrice;
        this.isValid = isValid;
        this.selectedComponents = selectedComponents;
    }

    public CarConfiguration getCarConfiguration() {
        return configuration;
    }
    public BigDecimal getTotalPrice() {
        return totalPrice;
    }
    public boolean isValid() {
        return isValid;
    }
    public Map<ComponentType, Component> getSelectedComponents() {
        return selectedComponents;
    }
}
