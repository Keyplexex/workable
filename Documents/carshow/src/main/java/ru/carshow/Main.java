package ru.carshow;

import ru.carshow.domain.model.car.*;
import ru.carshow.domain.model.configuration.Component;
import ru.carshow.domain.model.configuration.ComponentType;
import ru.carshow.domain.model.configuration.ConfigurationResult;
import ru.carshow.domain.model.order.InStockOrder;
import ru.carshow.domain.model.order.Order;
import ru.carshow.domain.model.order.OrderStatus;
import ru.carshow.domain.model.part.Part;
import ru.carshow.domain.model.part.PartCategory;
import ru.carshow.domain.model.testdrive.TestDriveRequest;
import ru.carshow.domain.model.user.*;
import ru.carshow.repository.*;
import ru.carshow.service.*;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.List;
import java.util.Map;
import java.util.Optional;

public class Main {
    public static void main(String[] args) {
        System.out.println("=== Автосалон: Система управления ===");
        
        UserRepository userRepository = new UserRepository();
        CarModelRepository carModelRepository = new CarModelRepository();
        CarRepository carRepository = new CarRepository();
        PartRepository partRepository = new PartRepository();
        OrderRepository orderRepository = new OrderRepository();
        TestDriveRepository testDriveRepository = new TestDriveRepository();
        ComponentRepository componentRepository = new ComponentRepository();
        
        UserService userService = new UserService(userRepository);
        CarService carService = new CarService(carRepository, carModelRepository);
        PartService partService = new PartService(partRepository, carModelRepository);
        OrderService orderService = new OrderService(orderRepository, userRepository, carRepository);
        TestDriveService testDriveService = new TestDriveService(testDriveRepository, userRepository, carRepository);
        ConfigurationService configurationService = new ConfigurationService(carModelRepository, componentRepository);
        
        initializeData(userService, carModelRepository, carService, partService, componentRepository);
        
        demonstrateSystem(userService, carService, orderService, testDriveService, configurationService, partService, carModelRepository);
    }
    
    private static void initializeData(UserService userService,
                                       CarModelRepository carModelRepository,
                                       CarService carService,
                                       PartService partService,
                                       ComponentRepository componentRepository) {
        
        System.out.println("\n--- Инициализация данных ---");
        
        Client client1 = userService.createClient("Иван Петров", "ivan@email.com", "+79001234567");
        Client client2 = userService.createClient("Мария Сидорова", "maria@email.com", "+79007654321");
        
        Manager manager1 = userService.createManager("Алексей Иванов", "a.ivanov@autosalon.ru", "+79111111111");
        Manager manager2 = userService.createManager("Елена Смирнова", "e.smirnova@autosalon.ru", "+79112222222");
        
        WarehouseAdmin warehouseAdmin = userService.createWarehouseAdmin("Петр Соколов", "p.sokolov@autosalon.ru", "+79113333333");
        SystemAdmin systemAdmin = userService.createSystemAdmin("Дмитрий Орлов", "d.orlov@autosalon.ru", "+79114444444");
        
        System.out.println("Созданы пользователи:");
        System.out.println("- Клиенты: " + client1.getName() + ", " + client2.getName());
        System.out.println("- Менеджеры: " + manager1.getName() + ", " + manager2.getName());
        
        CarModel bmw320i = new CarModel(
            carModelRepository.generateId(),
            CarBrand.BMW,
            "320i",
            BodyType.SEDAN,
            new BigDecimal("3500000")
        );
        carModelRepository.save(bmw320i);
        
        CarModel bmw330i = new CarModel(
            carModelRepository.generateId(),
            CarBrand.BMW,
            "330i",
            BodyType.SEDAN,
            new BigDecimal("4200000")
        );
        carModelRepository.save(bmw330i);
        
        CarModel bmwM340i = new CarModel(
            carModelRepository.generateId(),
            CarBrand.BMW,
            "M340i",
            BodyType.SEDAN,
            new BigDecimal("5500000")
        );
        carModelRepository.save(bmwM340i);
        
        CarModel toyotaCamry = new CarModel(
            carModelRepository.generateId(),
            CarBrand.TOYOTA,
            "Camry",
            BodyType.SEDAN,
            new BigDecimal("2800000")
        );
        carModelRepository.save(toyotaCamry);
        
        System.out.println("Созданы модели автомобилей:");
        System.out.println("- BMW 320i (базовая цена: 3 500 000 ₽)");
        System.out.println("- BMW 330i (базовая цена: 4 200 000 ₽)");
        System.out.println("- BMW M340i (базовая цена: 5 500 000 ₽)");
        System.out.println("- Toyota Camry (базовая цена: 2 800 000 ₽)");
        
        Car car1 = carService.createCar(
            bmw320i.getId(), "WBA12345678901234", 2023, "Черный",
            184, 2.0, FuelType.PETROL, TransmissionType.AUTOMATIC,
            DriveType.REAR, new BigDecimal("3700000")
        );
        
        Car car2 = carService.createCar(
            bmw330i.getId(), "WBA23456789012345", 2024, "Белый",
            245, 2.0, FuelType.PETROL, TransmissionType.AUTOMATIC,
            DriveType.REAR, new BigDecimal("4400000")
        );
        
        Car car3 = carService.createCar(
            toyotaCamry.getId(), "JTN12345678901234", 2023, "Серебристый",
            181, 2.5, FuelType.PETROL, TransmissionType.AUTOMATIC,
            DriveType.FRONT, new BigDecimal("2900000")
        );
        
        System.out.println("Созданы автомобили в наличии:");
        System.out.println("- BMW 320i (VIN: " + car1.getVin() + ", цена: " + car1.getPrice() + " ₽)");
        System.out.println("- BMW 330i (VIN: " + car2.getVin() + ", цена: " + car2.getPrice() + " ₽)");
        System.out.println("- Toyota Camry (VIN: " + car3.getVin() + ", цена: " + car3.getPrice() + " ₽)");
        
        Part wheels17Standard = partService.createPart(
            "17'' Standard", PartCategory.WHEELS, new BigDecimal("0"),
            "BMW", "WH17-STD", 10
        );
        wheels17Standard.addCompatibleCarModel(bmw320i.getId());
        partService.addCompatibility(wheels17Standard.getId(), bmw320i.getId());
        
        Part wheels19MSport = partService.createPart(
            "19'' M-Sport", PartCategory.WHEELS, new BigDecimal("95000"),
            "BMW", "WH19-MSP", 5
        );
        wheels19MSport.addCompatibleCarModel(bmw320i.getId());
        wheels19MSport.addCompatibleCarModel(bmw330i.getId());
        wheels19MSport.addCompatibleCarModel(bmwM340i.getId());
        partService.addCompatibility(wheels19MSport.getId(), bmw320i.getId());
        partService.addCompatibility(wheels19MSport.getId(), bmw330i.getId());
        partService.addCompatibility(wheels19MSport.getId(), bmwM340i.getId());
        
        Part wheels18Aero = partService.createPart(
            "18'' Aero", PartCategory.WHEELS, new BigDecimal("45000"),
            "BMW", "WH18-AERO", 3
        );
        wheels18Aero.addCompatibleCarModel(bmw320i.getId());
        wheels18Aero.addCompatibleCarModel(bmw330i.getId());
        partService.addCompatibility(wheels18Aero.getId(), bmw320i.getId());
        partService.addCompatibility(wheels18Aero.getId(), bmw330i.getId());
        
        Part transmissionAuto8AT = partService.createPart(
            "Автоматическая 8AT", PartCategory.TRANSMISSION, new BigDecimal("0"),
            "ZF", "TR-8AT", 8
        );
        transmissionAuto8AT.addCompatibleCarModel(bmw320i.getId());
        transmissionAuto8AT.addCompatibleCarModel(bmw330i.getId());
        partService.addCompatibility(transmissionAuto8AT.getId(), bmw320i.getId());
        partService.addCompatibility(transmissionAuto8AT.getId(), bmw330i.getId());
        
        Part transmissionManual6MT = partService.createPart(
            "Механическая 6MT", PartCategory.TRANSMISSION, new BigDecimal("-30000"),
            "ZF", "TR-6MT", 4
        );
        transmissionManual6MT.addCompatibleCarModel(bmw320i.getId());
        transmissionManual6MT.addCompatibleCarModel(bmw330i.getId());
        partService.addCompatibility(transmissionManual6MT.getId(), bmw320i.getId());
        partService.addCompatibility(transmissionManual6MT.getId(), bmw330i.getId());
        
        Part steeringWheelStandard = partService.createPart(
            "Спортивный кожаный (Standard)", PartCategory.STEERING_WHEEL, new BigDecimal("0"),
            "BMW", "SW-STD", 12
        );
        steeringWheelStandard.addCompatibleCarModel(bmw320i.getId());
        steeringWheelStandard.addCompatibleCarModel(bmw330i.getId());
        partService.addCompatibility(steeringWheelStandard.getId(), bmw320i.getId());
        partService.addCompatibility(steeringWheelStandard.getId(), bmw330i.getId());
        
        Part steeringWheelMSport = partService.createPart(
            "M-Sport с подогревом", PartCategory.STEERING_WHEEL, new BigDecimal("25000"),
            "BMW", "SW-MSP", 6
        );
        steeringWheelMSport.addCompatibleCarModel(bmw320i.getId());
        steeringWheelMSport.addCompatibleCarModel(bmw330i.getId());
        steeringWheelMSport.addCompatibleCarModel(bmwM340i.getId());
        partService.addCompatibility(steeringWheelMSport.getId(), bmw320i.getId());
        partService.addCompatibility(steeringWheelMSport.getId(), bmw330i.getId());
        partService.addCompatibility(steeringWheelMSport.getId(), bmwM340i.getId());
        
        Part interiorGraphite = partService.createPart(
            "Тканевый Graphite", PartCategory.INTERIOR, new BigDecimal("0"),
            "BMW", "IN-GRAPH", 7
        );
        interiorGraphite.addCompatibleCarModel(bmw320i.getId());
        partService.addCompatibility(interiorGraphite.getId(), bmw320i.getId());
        
        Part interiorDakota = partService.createPart(
            "Кожаный Dakota", PartCategory.INTERIOR, new BigDecimal("110000"),
            "BMW", "IN-DAK", 4
        );
        interiorDakota.addCompatibleCarModel(bmw320i.getId());
        interiorDakota.addCompatibleCarModel(bmw330i.getId());
        partService.addCompatibility(interiorDakota.getId(), bmw320i.getId());
        partService.addCompatibility(interiorDakota.getId(), bmw330i.getId());
        
        Part interiorPerformance = partService.createPart(
            "Спортивный Performance", PartCategory.INTERIOR, new BigDecimal("160000"),
            "BMW", "IN-PERF", 3
        );
        interiorPerformance.addCompatibleCarModel(bmw330i.getId());
        interiorPerformance.addCompatibleCarModel(bmwM340i.getId());
        partService.addCompatibility(interiorPerformance.getId(), bmw330i.getId());
        partService.addCompatibility(interiorPerformance.getId(), bmwM340i.getId());
        
        Component wheels17StdComp = new Component(
            componentRepository.generateId(),
            "17'' Standard", ComponentType.WHEELS,
            "Стандартные 17-дюймовые колеса",
            new BigDecimal("0"), wheels17Standard, true
        );
        componentRepository.save(wheels17StdComp);
        
        Component wheels19MSportComp = new Component(
            componentRepository.generateId(),
            "19'' M-Sport", ComponentType.WHEELS,
            "Спортивные 19-дюймовые колеса M-Sport",
            new BigDecimal("95000"), wheels19MSport, false
        );
        componentRepository.save(wheels19MSportComp);
        
        Component wheels18AeroComp = new Component(
            componentRepository.generateId(),
            "18'' Aero", ComponentType.WHEELS,
            "Аэродинамические 18-дюймовые колеса",
            new BigDecimal("45000"), wheels18Aero, false
        );
        componentRepository.save(wheels18AeroComp);
        
        Component transmissionAuto8ATComp = new Component(
            componentRepository.generateId(),
            "Автоматическая 8AT", ComponentType.TRANSMISSION,
            "8-ступенчатая автоматическая коробка передач",
            new BigDecimal("0"), transmissionAuto8AT, true
        );
        componentRepository.save(transmissionAuto8ATComp);
        
        Component transmissionManual6MTComp = new Component(
            componentRepository.generateId(),
            "Механическая 6MT", ComponentType.TRANSMISSION,
            "6-ступенчатая механическая коробка передач",
            new BigDecimal("-30000"), transmissionManual6MT, false
        );
        componentRepository.save(transmissionManual6MTComp);
        
        Component steeringWheelStdComp = new Component(
            componentRepository.generateId(),
            "Спортивный кожаный (Standard)", ComponentType.STEERING_WHEEL,
            "Спортивный кожаный руль",
            new BigDecimal("0"), steeringWheelStandard, true
        );
        componentRepository.save(steeringWheelStdComp);
        
        Component steeringWheelMSportComp = new Component(
            componentRepository.generateId(),
            "M-Sport с подогревом", ComponentType.STEERING_WHEEL,
            "Руль M-Sport с функцией подогрева",
            new BigDecimal("25000"), steeringWheelMSport, false
        );
        componentRepository.save(steeringWheelMSportComp);
        
        Component interiorGraphiteComp = new Component(
            componentRepository.generateId(),
            "Тканевый Graphite", ComponentType.INTERIOR,
            "Тканевый салон Graphite",
            new BigDecimal("0"), interiorGraphite, true
        );
        componentRepository.save(interiorGraphiteComp);
        
        Component interiorDakotaComp = new Component(
            componentRepository.generateId(),
            "Кожаный Dakota", ComponentType.INTERIOR,
            "Кожаный салон Dakota",
            new BigDecimal("110000"), interiorDakota, false
        );
        componentRepository.save(interiorDakotaComp);
        
        Component interiorPerformanceComp = new Component(
            componentRepository.generateId(),
            "Спортивный Performance", ComponentType.INTERIOR,
            "Спортивный салон Performance",
            new BigDecimal("160000"), interiorPerformance, false
        );
        componentRepository.save(interiorPerformanceComp);
        
        System.out.println("Созданы запчасти и компоненты для конфигуратора");
    }
    
    private static void demonstrateSystem(UserService userService,
                                          CarService carService,
                                          OrderService orderService,
                                          TestDriveService testDriveService,
                                          ConfigurationService configurationService,
                                          PartService partService,
                                          CarModelRepository carModelRepository) {
        
        System.out.println("\n=== Демонстрация работы системы ===");
        
        System.out.println("\n--- 1. Просмотр доступных автомобилей ---");
        List<Car> availableCars = carService.getAvailableCars();
        availableCars.forEach(car -> {
            System.out.println(car.getModel().getBrand() + " " + car.getModel().getName() +
                ", цвет: " + car.getColor() + ", цена: " + car.getPrice() + " ₽");
        });
        
        System.out.println("\n--- 2. Фильтрация автомобилей (BMW, цена до 4 000 000) ---");
        CarRepository.CarFilter filter = new CarRepository.CarFilter()
            .withBrand(CarBrand.BMW)
            .withMaxPrice(new BigDecimal("4000000"));
        
        List<Car> filteredCars = carService.findCarsByFilters(filter);
        filteredCars.forEach(car -> {
            System.out.println(car.getModel().getBrand() + " " + car.getModel().getName() +
                ", цена: " + car.getPrice() + " ₽");
        });
        
        System.out.println("\n--- 3. Создание заказа на автомобиль в наличии ---");
        Client client = (Client) userService.getAllUsers().stream()
            .filter(u -> u instanceof Client)
            .findFirst()
            .orElseThrow();
        
        Car carToOrder = carService.getAvailableCars().get(0);
        
        InStockOrder order = orderService.createInStockOrder(client.getId(), carToOrder.getId());
        System.out.println("Создан заказ #" + order.getId() + 
            " на автомобиль " + carToOrder.getModel().getBrand() + " " + carToOrder.getModel().getName());
        System.out.println("Статус заказа: " + order.getStatus());
        System.out.println("Менеджер: " + order.getManager().getName());
        
        System.out.println("\n--- 4. Обновление статуса заказа ---");
        order.setStatus(OrderStatus.CONFIRMED_BY_WAREHOUSE);
        System.out.println("Новый статус заказа: " + order.getStatus());
        
        System.out.println("\n--- 5. Создание заявки на тест-драйв ---");
        Car testDriveCar = carService.getAvailableCars().get(1);
        LocalDateTime testDriveTime = LocalDateTime.now().plusDays(2).withHour(14).withMinute(0);
        
        TestDriveRequest testDriveRequest = testDriveService.createRequest(
            client.getId(), 
            testDriveCar.getId(), 
            testDriveTime
        );
        System.out.println("Создана заявка на тест-драйв #" + testDriveRequest.getId());
        System.out.println("Автомобиль: " + testDriveCar.getModel().getBrand() + " " + testDriveCar.getModel().getName());
        System.out.println("Дата и время: " + testDriveRequest.getStartDateTime());
        System.out.println("Статус: " + testDriveRequest.getStatus());
        
        System.out.println("\n--- 6. Подтверждение заявки на тест-драйв ---");
        testDriveService.confirmRequest(testDriveRequest.getId());
        System.out.println("Заявка подтверждена. Новый статус: " + testDriveRequest.getStatus());
        
        System.out.println("\n--- 7. Конфигуратор: успешная сборка BMW 320i ---");
        
        CarModel bmw320i = carModelRepository.findAll().stream()
            .filter(m -> m.getBrand() == CarBrand.BMW && m.getName().equals("320i"))
            .findFirst()
            .orElseThrow();
        
        var availableComponents = configurationService.getAllAvailableComponents(bmw320i.getId());
        
        Component wheels19 = availableComponents.get(ComponentType.WHEELS).stream()
            .filter(c -> c.getName().contains("19"))
            .findFirst()
            .orElseThrow();
            
        Component transmission = availableComponents.get(ComponentType.TRANSMISSION).stream()
            .filter(c -> c.getName().contains("Автоматическая"))
            .findFirst()
            .orElseThrow();
            
        Component steeringWheel = availableComponents.get(ComponentType.STEERING_WHEEL).stream()
            .filter(c -> c.getName().contains("M-Sport"))
            .findFirst()
            .orElseThrow();
            
        Component interior = availableComponents.get(ComponentType.INTERIOR).stream()
            .filter(c -> c.getName().contains("Кожаный"))
            .findFirst()
            .orElseThrow();
        
        Map<ComponentType, Long> selectedComponents = Map.of(
            ComponentType.WHEELS, wheels19.getId(),
            ComponentType.TRANSMISSION, transmission.getId(),
            ComponentType.STEERING_WHEEL, steeringWheel.getId(),
            ComponentType.INTERIOR, interior.getId()
        );
        
        var result = configurationService.buildConfiguration(bmw320i.getId(), selectedComponents);
        
        if (result.isValid()) {
            System.out.println("Конфигурация успешно собрана!");
            System.out.println("Базовая цена: " + bmw320i.getBasePrice() + " ₽");
            System.out.println("Доплата за компоненты: " + 
                result.getTotalPrice().subtract(bmw320i.getBasePrice()) + " ₽");
            System.out.println("Итоговая цена: " + result.getTotalPrice() + " ₽");
            
            result.getSelectedComponents().forEach((type, component) -> {
                System.out.println("  - " + type + ": " + component.getName() + 
                    " (" + component.getSurcharge() + " ₽)");
            });
        } else {
            System.out.println("Конфигурация невалидна!");
        }
        
        System.out.println("\n--- 8. Конфигуратор: ошибка совместимости (интерьер Performance для 320i) ---");
        
        Optional<Component> performanceOpt = availableComponents.get(ComponentType.INTERIOR).stream()
            .filter(c -> c.getName().contains("Performance"))
            .findFirst();
        
        if (performanceOpt.isPresent()) {
            Component interiorPerf = performanceOpt.get();
            Map<ComponentType, Long> invalidComponents = Map.of(
                ComponentType.WHEELS, wheels19.getId(),
                ComponentType.TRANSMISSION, transmission.getId(),
                ComponentType.STEERING_WHEEL, steeringWheel.getId(),
                ComponentType.INTERIOR, interiorPerf.getId()
            );
            
            try {
                var invalidResult = configurationService.buildConfiguration(bmw320i.getId(), invalidComponents);
                if (!invalidResult.isValid()) {
                    System.out.println("ОШИБКА: Выбранный интерьер недоступен для модели BMW 320i");
                }
            } catch (ru.carshow.domain.exception.IncompatibleComponentException e) {
                System.out.println("Перехвачено исключение: " + e.getMessage());
            }
        } else {
            System.out.println("Компонент 'Спортивный Performance' отсутствует в списке доступных для BMW 320i (ожидаемая ошибка совместимости)");
        }
        
        System.out.println("\n--- 9. Просмотр заказов менеджером ---");
        Manager manager = (Manager) userService.getAllUsers().stream()
            .filter(u -> u instanceof Manager)
            .findFirst()
            .orElseThrow();
        
        List<Order> managerOrders = orderService.getOrdersByManager(manager.getId());
        System.out.println("Менеджер " + manager.getName() + " ведет " + 
            managerOrders.size() + " заказов");
        

        System.out.println("\n--- 10. Работа администратора склада ---");
        System.out.println("Всего запчастей на складе: " + 
            partService.getAllParts().size());
        System.out.println("Запчасти категории WHEELS: " + 
            partService.getPartsByCategory(PartCategory.WHEELS).size());
        
        System.out.println("\n--- 11. Отмена заказа ---");
        orderService.cancelOrder(order.getId());
        System.out.println("Заказ #" + order.getId() + " отменен. Статус: " + order.getStatus());
        
        System.out.println("\n=== Демонстрация завершена ===");
    }
}