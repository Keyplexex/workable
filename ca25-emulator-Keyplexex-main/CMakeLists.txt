cmake_minimum_required(VERSION 3.20)
project(riscv_sim LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(riscv-sim
    main.cpp
    io.cpp
    memory.cpp
    cache.cpp
    cpu.cpp
)

# Жёсткие предупреждения в debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(riscv-sim PRIVATE -Wall -Wextra -Wpedantic)
endif()
