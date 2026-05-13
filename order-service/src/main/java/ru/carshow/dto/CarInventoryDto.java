package ru.carshow.dto;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.UUID;

@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class CarInventoryDto {
    private UUID id;
    private String vin;
    private Integer year;
    private String color;
    private String enginePower;
    private String engineVolume;
    private String fuelType;
    private String transmission;
    private String driveType;
    private String price;
    private String status;
    private UUID modelId;
    private String modelBrand;
    private String modelName;
}
