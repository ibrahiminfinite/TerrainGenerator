cmake_minimum_required(VERSION 3.20)
project(TerrainGenerator)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_EXAMPLE "Build with example" ON)
option(BUILD_TEST "Build with tests" OFF)

include_directories(include)

find_package(Eigen3 REQUIRED)

set(required_components gui-osg collision-ode)
set(dart_required_libraries dart dart-gui-osg dart-collision-ode)
find_package(DART 6.9.2   REQUIRED COMPONENTS ${required_components})

add_library(TerrainGenerator
        src/TerrainGenerator.cpp)
target_link_libraries(TerrainGenerator PUBLIC Eigen3::Eigen ${dart_required_libraries})


add_executable(Example  examples/example.cpp)
target_link_libraries(Example TerrainGenerator )

add_executable(ExampleDart  examples/example_dart.cpp)
target_link_libraries(ExampleDart TerrainGenerator )