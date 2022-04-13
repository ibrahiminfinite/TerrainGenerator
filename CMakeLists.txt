cmake_minimum_required(VERSION 3.20)
project(TerrainGenerator)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_EXAMPLE "Build with example" OFF)
option(BUILD_TEST "Build with tests" OFF)

include_directories(include)

find_package(Eigen3 REQUIRED)

add_library(TerrainGenerator
        src/TerrainGenerator.cpp
        src/PerlinNoise.cpp)
target_link_libraries(TerrainGenerator PUBLIC Eigen3::Eigen)

if(BUILD_EXAMPLE)
    add_executable(Example  examples/example.cpp)
    target_link_libraries(Example TerrainGenerator)
endif()

if(BUILD_TESTS)
    add_executable(TimingTest tests/TEST_Generation_Timing.cpp)
    target_link_libraries(TimingTest TerrainGenerator)
endif()