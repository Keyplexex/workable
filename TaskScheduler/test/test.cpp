#include <gtest/gtest.h>
#include "scheduler.h"
#include <cmath>
#include <stdexcept>


// 1. Простое выполнение без зависимостей
TEST(TaskSchedulerTest, SimpleExecution) {
    TTaskScheduler scheduler;

    auto id = scheduler.add([](int a, int b) { return a + b; }, 2, 3);
    int result = scheduler.getResult<int>(id);

    EXPECT_EQ(result, 5);
}

// 2. Выполнение с зависимостью (Future)
TEST(TaskSchedulerTest, ExecutionWithDependency) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a, int b) { return a + b; }, 2, 3);
    auto id2 = scheduler.add([](int v) { return v * v; },
                              scheduler.getFutureResult<int>(id1));

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id1), 5);
    EXPECT_EQ(scheduler.getResult<int>(id2), 25);
}

// 3. Несколько зависимостей (fan-in)
TEST(TaskSchedulerTest, MultipleDependencies) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a) { return a * 2; }, 4); // 8
    auto id2 = scheduler.add([](int b) { return b + 3; }, 7); // 10

    auto id3 = scheduler.add([](int x, int y) { return x + y; },
                              scheduler.getFutureResult<int>(id1),
                              scheduler.getFutureResult<int>(id2));

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id1), 8);
    EXPECT_EQ(scheduler.getResult<int>(id2), 10);
    EXPECT_EQ(scheduler.getResult<int>(id3), 18);
}

// 4. Fan-out (одна задача используется в нескольких других)
TEST(TaskSchedulerTest, FanOutDependencies) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a, int b) { return a + b; }, 2, 3); // 5
    auto id2 = scheduler.add([](int v) { return v * 2; },
                              scheduler.getFutureResult<int>(id1)); // 10
    auto id3 = scheduler.add([](int v) { return v * 3; },
                              scheduler.getFutureResult<int>(id1)); // 15

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id1), 5);
    EXPECT_EQ(scheduler.getResult<int>(id2), 10);
    EXPECT_EQ(scheduler.getResult<int>(id3), 15);
}

// 5. Проверка на цикл (прямая самозависимость)
TEST(TaskSchedulerTest, DetectSelfCycle) {
    TTaskScheduler scheduler;

    // Задача ссылается сама на себя
    auto id1 = scheduler.add([](int v) { return v; },
                              scheduler.getFutureResult<int>(0));

    EXPECT_THROW(scheduler.executeAll(), const char*);
}

// 6. Проверка на цикл (косвенный цикл A->B->A)
TEST(TaskSchedulerTest, DetectIndirectCycle) {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int x) { return x + 1; },
                              scheduler.getFutureResult<int>(1)); // зависит от id2
    auto id2 = scheduler.add([](int y) { return y + 1; },
                              scheduler.getFutureResult<int>(0)); // зависит от id1

    EXPECT_THROW(scheduler.executeAll(), const char*);
}

// 7. Проверка работы с указателем на метод класса
struct AddNumber {
    int base;
    explicit AddNumber(int b) : base(b) {}
    int add(int x) const { return base + x; }
};

TEST(TaskSchedulerTest, MemberFunctionExecution) {
    TTaskScheduler scheduler;
    AddNumber adder(10);

    auto id = scheduler.add(&AddNumber::add, adder, 5);

    scheduler.executeAll();

    EXPECT_EQ(scheduler.getResult<int>(id), 15);
}
