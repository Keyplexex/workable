package ru.carshow.entity;

public enum OrderStatus {

    DRAFT,           
    AGREED,            
    PAYMENT_PENDING,   
    PAID,            
    READY_FOR_PICKUP,  
    COMPLETED,       
    CANCELLED,          
    WAREHOUSE_APPROVED, 
    WAITING_FOR_DELIVERY 
}