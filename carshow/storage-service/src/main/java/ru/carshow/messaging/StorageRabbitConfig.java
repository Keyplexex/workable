package ru.carshow.messaging;

import org.springframework.amqp.core.Binding;
import org.springframework.amqp.core.BindingBuilder;
import org.springframework.amqp.core.Queue;
import org.springframework.amqp.core.TopicExchange;
import org.springframework.amqp.rabbit.connection.ConnectionFactory;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.converter.SimpleMessageConverter;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import ru.carshow.events.RabbitNames;

@Configuration
public class StorageRabbitConfig {

    @Bean
    public TopicExchange carshowExchange() {
        return new TopicExchange(RabbitNames.EXCHANGE, true, false);
    }

    @Bean
    public Queue storageOrderInQueue() {
        return new Queue(RabbitNames.Q_STORAGE_ORDER_IN, true);
    }

    @Bean
    public Binding storageOrderInBinding(TopicExchange carshowExchange, Queue storageOrderInQueue) {
        return BindingBuilder.bind(storageOrderInQueue).to(carshowExchange).with(RabbitNames.RK_ORDER_SENT);
    }

    @Bean
    public RabbitTemplate rabbitTemplate(ConnectionFactory connectionFactory) {
        RabbitTemplate template = new RabbitTemplate(connectionFactory);
        template.setMessageConverter(new SimpleMessageConverter());
        return template;
    }
}
