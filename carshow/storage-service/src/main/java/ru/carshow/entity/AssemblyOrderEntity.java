package ru.carshow.entity;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Table;
import org.hibernate.annotations.JdbcTypeCode;
import org.hibernate.type.SqlTypes;
import lombok.Getter;
import lombok.Setter;

import java.util.UUID;

@Getter
@Setter
@Entity
@Table(name = "assembly_orders")
public class AssemblyOrderEntity extends BaseEntity {

    @Column(name = "source_order_id", nullable = false)
    private UUID sourceOrderId;

    @Column(name = "source_order_type", nullable = false, length = 50)
    private String sourceOrderType;

    @Column(name = "car_id")
    private UUID carId;

    @Column(name = "car_model_id")
    private UUID carModelId;

    @JdbcTypeCode(SqlTypes.JSON)
    @Column(name = "required_parts_json", columnDefinition = "jsonb")
    private String requiredPartsJson;

    @Column(name = "warehouse_operator_id")
    private UUID warehouseOperatorId;

    @Column(length = 50)
    private String status;

    public AssemblyOrderEntity() {}
}
