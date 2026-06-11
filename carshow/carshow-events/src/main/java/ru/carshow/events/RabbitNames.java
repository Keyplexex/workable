package ru.carshow.events;

public final class RabbitNames {

    public static final String EXCHANGE = "carshow.events";

    public static final String RK_ORDER_SENT = "order.sent-for-approval";
    public static final String RK_ORDER_APPROVED = "order.approved";
    public static final String RK_ORDER_REJECTED = "order.rejected";

    public static final String Q_STORAGE_ORDER_IN = "storage.order.sent-for-approval";
    public static final String Q_ORDER_RESULT_IN = "order.storage-result";

    private RabbitNames() {}
}
