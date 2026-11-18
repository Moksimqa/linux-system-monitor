cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(LinuxSystemMonitor LANGUAGES CXX)

add_executable(monitor
    src/main.cpp src/CpuCollector.cpp src/MemCollector.cpp src/NetCollector.cpp src/ConsoleRenderer.cpp src/ProcessCollector.cpp src/ProcessParser.cpp
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

add_executable(process_parser_tests
    tests/ProcessParserTests.cpp
    src/ProcessParser.cpp
)

target_include_directories(process_parser_tests PRIVATE include)

target_compile_features(process_parser_tests PRIVATE cxx_std_17)

target_compile_options(process_parser_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(process_parser_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(process_parser_tests)



option(ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)

if(ENABLE_SANITIZERS)
    target_compile_options(monitor PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(monitor PRIVATE
        -fsanitize=address,undefined
    )

    target_compile_options(process_parser_tests PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(process_parser_tests PRIVATE
        -fsanitize=address,undefined
    )
endif()
