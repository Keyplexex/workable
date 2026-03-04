package ru.carshow.domain.model.testdrive;

import ru.carshow.domain.model.car.Car;
import ru.carshow.domain.model.user.Client;
import java.time.LocalDateTime;
import java.util.Objects;

public class TestDriveRequest {
    private final Long id;
    private final Client client;
    private final Car car;
    private final LocalDateTime startDateTime;
    private TestDriveStatus status;
    
    public enum TestDriveStatus {
        PENDING, CONFIRMED, COMPLETED, CANCELLED
    }
    
    public TestDriveRequest(Long id, Client client, Car car, LocalDateTime startDateTime) {
        this.id = id;
        this.client = client;
        this.car = car;
        this.startDateTime = startDateTime;
        this.status = TestDriveStatus.PENDING;
    }
    
    public Long getId() { return id; }
    public Client getClient() { return client; }
    public Car getCar() { return car; }
    public LocalDateTime getStartDateTime() { return startDateTime; }
    public TestDriveStatus getStatus() { return status; }
    
    public void setStatus(TestDriveStatus status) {
        this.status = status;
    }
    
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        TestDriveRequest that = (TestDriveRequest) o;
        return Objects.equals(id, that.id);
    }
    
    @Override
    public int hashCode() {
        return Objects.hash(id);
    }
}