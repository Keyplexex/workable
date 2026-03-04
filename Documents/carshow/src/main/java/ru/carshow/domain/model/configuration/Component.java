package ru.carshow.domain.model.configuration;

import ru.carshow.domain.model.part.Part;

import java.math.BigDecimal;
import java.util.Objects;
import java.util.Set;

public class Component {
    private final Long id;
    private final String name;
    private final ComponentType type;
    private final String description;
    private final BigDecimal surcharge;
    private final Part part;
    private final boolean isDefault;
    
    public Component(Long id, String name, ComponentType type, String description, 
                     BigDecimal surcharge, Part part, boolean isDefault) {
        this.id = id;
        this.name = name;
        this.type = type;
        this.description = description;
        this.surcharge = surcharge;
        this.part = part;
        this.isDefault = isDefault;
    }
    
    public Long getId() { 
        return id; 
    }
    public String getName() { 
        return name; 
    }
    public ComponentType getType() { 
        return type; 
    }
    public String getDescription() { 
        return description; 
    }
    public BigDecimal getSurcharge() { 
        return surcharge; 
    }
    public Part getPart() { 
        return part; 
    }
    public boolean isDefault() { 
        return isDefault; 
    }
    
    public boolean isCompatibleWithCarModel(Long carModelId) {
        return part.isCompatibleWithCarModel(carModelId);
    }
    
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Component component = (Component) o;
        return Objects.equals(id, component.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}