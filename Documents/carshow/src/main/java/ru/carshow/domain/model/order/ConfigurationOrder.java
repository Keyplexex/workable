package ru.carshow.domain.model.order;

import ru.carshow.domain.model.car.CarModel;
import ru.carshow.domain.model.configuration.CarConfiguration;
import ru.carshow.domain.model.user.Client;
import ru.carshow.domain.model.user.Manager;

public class ConfigurationOrder extends Order {
    private final CarModel carModel;
    private CarConfiguration configuration;
    
    public ConfigurationOrder(Long id, Client client, Manager manager, CarModel carModel) {
        super(id, client, manager);
        this.carModel = carModel;
    }
    
    public CarModel getCarModel() { 
        return carModel; 
    }
    public CarConfiguration getConfiguration() { 
        return configuration; 
    }
    
    public void setConfiguration(CarConfiguration configuration) {
        this.configuration = configuration;
    }
    
    @Override
    public String getOrderType() {
        return "CONFIGURATION";
    }
}