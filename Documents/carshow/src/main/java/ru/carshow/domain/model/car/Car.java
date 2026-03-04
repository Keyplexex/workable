package ru.carshow.domain.model.car;

import java.math.BigDecimal;
import java.util.Objects;

import ru.carshow.domain.model.car.CarStatus;
import ru.carshow.domain.model.car.DriveType;
import ru.carshow.domain.model.car.FuelType;
import ru.carshow.domain.model.car.TransmissionType;

public class Car {
    private final Long id;
    private final CarModel model;
    private String vin;
    private int year;
    private String color;
    private double enginePower;
    private double engineVolume;
    private FuelType fuelType;
    private TransmissionType transmission;
    private DriveType driveType;
    private CarStatus status;
    private BigDecimal price;

    public Car(Long id, CarModel model, String vin, int year, String color, 
               double enginePower, double engineVolume, FuelType fuelType, 
               TransmissionType transmission, DriveType driveType,
               CarStatus status, BigDecimal price) {
        this.id = id;
        this.model = model;
        this.vin = vin;
        this.year = year;
        this.color = color;
        this.enginePower = enginePower;
        this.engineVolume = engineVolume;
        this.fuelType = fuelType;
        this.transmission = transmission;
        this.driveType = driveType;
        this.status = status;
        this.price = price;
    }

    public Long getId() {
        return id;
    }
    public CarModel getModel() {
        return model;
    }
    public String getVin() {
        return vin;       
    }
    public int getYear() {
        return year;
    }
    public String getColor() {
        return color;
    }
    public double getEnginePower() {
        return enginePower;
    }
    public double getEngineVolume() {
        return engineVolume;
    }
    public FuelType getFuelType() {
        return fuelType;
    }
    public TransmissionType getTransmissionType() {
        return transmission;
    }
    public DriveType getDriveType() {
        return driveType;
    }
    public CarStatus getStatus() {
        return status;
    }
    public BigDecimal getPrice() {
        return price;
    }

    public void setVin(String vin) {
        this.vin = vin;       
    }
    public void setYear(int year) {
        this.year = year;
    }
    public void setColor(String color) {
        this.color = color;
    }
    public void setEnginePower(double enginePower) {
        this.enginePower = enginePower;
    }
    public void setEngineVolume(double engineVolume) {
        this.engineVolume = engineVolume;
    }
    public void setFuelType(FuelType fuelType) {
        this.fuelType = fuelType;
    }
    public void setTransmission(TransmissionType transmission) {
        this.transmission = transmission;
    }
    public void setDriveType(DriveType driveType) {
        this.driveType = driveType;
    }
    public void setStatus(CarStatus status) {
        this.status = status;
    }
    public void setPrice(BigDecimal price) {
        this.price = price;
    }

    @Override
    public boolean equals(Object c) {
        if (this == c) {
            return true;
        }
        if ((c == null) || getClass() != c.getClass()) {
            return false;
        }
        Car car = (Car) c;
        return Objects.equals(id, car.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}


