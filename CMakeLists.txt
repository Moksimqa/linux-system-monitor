cmake_minimum_required(VERSION 3.16)

project(LinuxSystemMonitor LANGUAGES CXX)

add_executable(monitor
    src/main.cpp src/CpuCollector.cpp src/MemCollector.cpp src/NetCollector.cpp src/ConsoleRenderer.cpp src/ProcessCollector.cpp
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
