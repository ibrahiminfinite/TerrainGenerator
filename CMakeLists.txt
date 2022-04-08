cmake_minimum_required(VERSION 3.20)
project(TerrainGenerator)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
add_executable(Main
        examples/example.cpp
        src/TerrainGenerator.cpp
        src/PerlinNoise.cpp)
