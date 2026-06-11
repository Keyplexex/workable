package ru.carshow.client;

import io.grpc.StatusRuntimeException;
import lombok.extern.slf4j.Slf4j;
import net.devh.boot.grpc.client.inject.GrpcClient;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;
import ru.carshow.grpc.inventory.CarIdRequest;
import ru.carshow.grpc.inventory.CarInventoryMessage;
import ru.carshow.grpc.inventory.CarInventoryServiceGrpc;
import ru.carshow.grpc.inventory.CarSingleResponse;
import ru.carshow.grpc.inventory.Empty;

import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.TimeUnit;

@Component
@Slf4j
public class CarInventoryGrpcClient {

    @GrpcClient("storage")
    private CarInventoryServiceGrpc.CarInventoryServiceBlockingStub carInventoryStub;

    @Value("${storage.grpc.deadline-ms:2000}")
    private long deadlineMs;

    public List<CarInventoryMessage> listAvailableCars() {
        log.info("gRPC client -> storage ListAvailableCars (deadline {} ms)", deadlineMs);
        try {
            return carInventoryStub
                    .withDeadlineAfter(deadlineMs, TimeUnit.MILLISECONDS)
                    .listAvailableCars(Empty.getDefaultInstance())
                    .getCarsList();
        } catch (StatusRuntimeException e) {
            log.warn("gRPC ListAvailableCars failed: {}", e.getStatus());
            throw e;
        }
    }

    public Optional<CarInventoryMessage> getAvailableCar(UUID id) {
        log.info("gRPC client -> storage GetAvailableCar id={} (deadline {} ms)", id, deadlineMs);
        try {
            CarSingleResponse response = carInventoryStub
                    .withDeadlineAfter(deadlineMs, TimeUnit.MILLISECONDS)
                    .getAvailableCar(CarIdRequest.newBuilder().setId(id.toString()).build());
            if (!response.getFound()) {
                return Optional.empty();
            }
            return Optional.of(response.getCar());
        } catch (StatusRuntimeException e) {
            log.warn("gRPC GetAvailableCar failed: {}", e.getStatus());
            throw e;
        }
    }
}
