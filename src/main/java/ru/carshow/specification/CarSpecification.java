package ru.carshow.specification;

import org.springframework.data.jpa.domain.Specification;
import ru.carshow.entity.CarEntity;
import java.math.BigDecimal;
import java.util.UUID;

public class CarSpecification {
    
    public static Specification<CarEntity> isNotRemoved() {
        return (root, query, cb) -> cb.isFalse(root.get("removed"));
    }
    
    public static Specification<CarEntity> hasBrand(String brand) {
        return (root, query, cb) -> 
            brand == null ? null : cb.equal(root.get("model").get("brand"), brand);
    }
    
    public static Specification<CarEntity> hasModel(String modelName) {
        return (root, query, cb) -> 
            modelName == null ? null : cb.equal(root.get("model").get("name"), modelName);
    }
    
    public static Specification<CarEntity> priceBetween(BigDecimal min, BigDecimal max) {
        return (root, query, cb) -> {
            if (min == null && max == null) return null;
            if (min == null) return cb.lessThanOrEqualTo(root.get("price"), max);
            if (max == null) return cb.greaterThanOrEqualTo(root.get("price"), min);
            return cb.between(root.get("price"), min, max);
        };
    }
    
    public static Specification<CarEntity> hasFuelType(String fuelType) {
        return (root, query, cb) -> 
            fuelType == null ? null : cb.equal(root.get("fuelType"), fuelType);
    }
    
    public static Specification<CarEntity> hasTransmission(String transmission) {
        return (root, query, cb) -> 
            transmission == null ? null : cb.equal(root.get("transmission"), transmission);
    }
    
    public static Specification<CarEntity> hasDriveType(String driveType) {
        return (root, query, cb) -> 
            driveType == null ? null : cb.equal(root.get("driveType"), driveType);
    }
    
    public static Specification<CarEntity> hasBodyType(String bodyType) {
        return (root, query, cb) -> 
            bodyType == null ? null : cb.equal(root.get("model").get("bodyType"), bodyType);
    }
    
    public static Specification<CarEntity> enginePowerBetween(BigDecimal min, BigDecimal max) {
        return (root, query, cb) -> {
            if (min == null && max == null) return null;
            if (min == null) return cb.lessThanOrEqualTo(root.get("enginePower"), max);
            if (max == null) return cb.greaterThanOrEqualTo(root.get("enginePower"), min);
            return cb.between(root.get("enginePower"), min, max);
        };
    }
}