package ru.carshow.security;

import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Component;
import ru.carshow.repository.OrderRepository;
import java.util.UUID;

@Component("orderSecurity") 
@RequiredArgsConstructor
public class OrderSecurity {

    private final OrderRepository orderRepository;
    private final CurrentUser currentUser;

    public boolean isOwner(UUID orderId) {
        return orderRepository.findById(orderId)
            .map(order -> order.getClient().getId().equals(currentUser.getId()))
            .orElse(false);
    }
}