cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(LinuxSystemMonitor LANGUAGES CXX)

add_executable(monitor
    src/main.cpp src/CpuCollector.cpp src/MemCollector.cpp src/NetCollector.cpp src/ConsoleRenderer.cpp src/ProcessCollector.cpp src/ProcessParser.cpp src/CliParser.cpp src/FileLogger.cpp
)

target_compile_features(monitor PRIVATE cxx_std_17)

target_include_directories(monitor PRIVATE include)

target_compile_options(monitor PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

find_package(Threads REQUIRED)

target_link_libraries(monitor PRIVATE Threads::Threads)






include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(monitor_tests
    tests/ProcessParserTests.cpp
    tests/MemCollectorTests.cpp
    tests/CpuCollectorTests.cpp
    tests/NetCollectorTests.cpp
    tests/CliParserTests.cpp

    src/ProcessParser.cpp
    src/MemCollector.cpp
    src/CpuCollector.cpp
    src/NetCollector.cpp
    src/CliParser.cpp
)

target_include_directories(monitor_tests PRIVATE include)

target_compile_features(monitor_tests PRIVATE cxx_std_17)

target_compile_options(monitor_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(monitor_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(monitor_tests)



option(ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)

if(ENABLE_SANITIZERS)
    target_compile_options(monitor PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(monitor PRIVATE
        -fsanitize=address,undefined
    )

    target_compile_options(monitor_tests PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(monitor_tests PRIVATE
        -fsanitize=address,undefined
    )
endif()
