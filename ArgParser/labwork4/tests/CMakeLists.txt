# Включение тестирования
enable_testing()

# Подключение googletest из локальной папки
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest-main ${CMAKE_BINARY_DIR}/googletest-build)

# Добавление тестового исполняемого файла
add_executable(
    argparser_tests
    argparser_test.cpp
)

# Линковка тестового исполняемого файла с библиотеками GoogleTest и вашей библиотекой ArgParser
target_link_libraries(
    argparser_tests
    argparser
    gtest_main
)

# Указание include-директории
target_include_directories(argparser_tests PUBLIC ${PROJECT_SOURCE_DIR})

# Интеграция с GoogleTest для автоматического обнаружения тестов
include(GoogleTest)
gtest_discover_tests(argparser_tests)
