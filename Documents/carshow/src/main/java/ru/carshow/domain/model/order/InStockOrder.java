package ru.carshow.domain.model.order;

import ru.carshow.domain.model.car.Car;
import ru.carshow.domain.model.user.Client;
import ru.carshow.domain.model.user.Manager;

public class InStockOrder extends Order{
    private final Car car;

    public InStockOrder(Long id, Client client, Manager manager, Car car) {
        super(id, client, manager);
        this.car = car;
    }

    public Car getCar() {
        return car;
    }

    @Override
    public String getOrderType() {
        return "IN_STOCK";
    }
    
}
