package ru.carshow.grpc;

import io.grpc.Status;
import io.grpc.stub.StreamObserver;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import net.devh.boot.grpc.server.service.GrpcService;
import org.springframework.transaction.annotation.Transactional;
import ru.carshow.entity.CarEntity;
import ru.carshow.entity.CarStatus;
import ru.carshow.grpc.inventory.CarIdRequest;
import ru.carshow.grpc.inventory.CarInventoryMessage;
import ru.carshow.grpc.inventory.CarInventoryServiceGrpc;
import ru.carshow.grpc.inventory.CarListResponse;
import ru.carshow.grpc.inventory.CarSingleResponse;
import ru.carshow.grpc.inventory.Empty;
import ru.carshow.repository.CarRepository;

import java.util.UUID;

@GrpcService
@RequiredArgsConstructor
@Slf4j
public class CarInventoryGrpcServiceImpl extends CarInventoryServiceGrpc.CarInventoryServiceImplBase {

    private final CarRepository carRepository;

    @Override
    @Transactional(readOnly = true)
    public void listAvailableCars(Empty request, StreamObserver<CarListResponse> responseObserver) {
        log.info("gRPC ListAvailableCars");
        try {
            var cars = carRepository.findAvailableForSaleWithModel(CarStatus.AVAILABLE);
            var builder = CarListResponse.newBuilder();
            for (CarEntity c : cars) {
                builder.addCars(toMessage(c));
            }
            responseObserver.onNext(builder.build());
            responseObserver.onCompleted();
        } catch (Exception e) {
            log.error("gRPC ListAvailableCars failed", e);
            responseObserver.onError(Status.INTERNAL.withCause(e).asRuntimeException());
        }
    }

    @Override
    @Transactional(readOnly = true)
    public void getAvailableCar(CarIdRequest request, StreamObserver<CarSingleResponse> responseObserver) {
        log.info("gRPC GetAvailableCar id={}", request.getId());
        final UUID id;
        try {
            id = UUID.fromString(request.getId());
        } catch (IllegalArgumentException e) {
            responseObserver.onError(Status.INVALID_ARGUMENT.withDescription("Invalid car id").asRuntimeException());
            return;
        }
        try {
            var opt = carRepository.findAvailableForSaleById(id, CarStatus.AVAILABLE);
            if (opt.isEmpty()) {
                responseObserver.onNext(CarSingleResponse.newBuilder().setFound(false).build());
            } else {
                responseObserver.onNext(
                        CarSingleResponse.newBuilder().setFound(true).setCar(toMessage(opt.get())).build());
            }
            responseObserver.onCompleted();
        } catch (Exception e) {
            log.error("gRPC GetAvailableCar failed", e);
            responseObserver.onError(Status.INTERNAL.withCause(e).asRuntimeException());
        }
    }

    private static CarInventoryMessage toMessage(CarEntity c) {
        var b = CarInventoryMessage.newBuilder()
                .setId(c.getId().toString())
                .setVin(c.getVin())
                .setYear(c.getYear())
                .setStatus(c.getStatus() != null ? c.getStatus().name() : "");
        if (c.getColor() != null) {
            b.setColor(c.getColor());
        }
        if (c.getEnginePower() != null) {
            b.setEnginePower(c.getEnginePower().toPlainString());
        }
        if (c.getEngineVolume() != null) {
            b.setEngineVolume(c.getEngineVolume().toPlainString());
        }
        if (c.getFuelType() != null) {
            b.setFuelType(c.getFuelType().name());
        }
        if (c.getTransmission() != null) {
            b.setTransmission(c.getTransmission().name());
        }
        if (c.getDriveType() != null) {
            b.setDriveType(c.getDriveType().name());
        }
        if (c.getPrice() != null) {
            b.setPrice(c.getPrice().toPlainString());
        }
        if (c.getModel() != null) {
            b.setModelId(c.getModel().getId().toString());
            if (c.getModel().getBrand() != null) {
                b.setModelBrand(c.getModel().getBrand());
            }
            if (c.getModel().getName() != null) {
                b.setModelName(c.getModel().getName());
            }
        }
        return b.build();
    }
}
