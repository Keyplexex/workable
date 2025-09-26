cmake_minimum_required(VERSION 3.10)

project(BattleshipGame)

set(CMAKE_CXX_STANDARD 17)

add_executable(battleship
    main.cpp
    Game.cpp
    GameField.cpp
)

target_include_directories(battleship PRIVATE .)

