#include <chrono>
#include <iostream>

#include "TerrainGenerator.h"

void testConfig(char* terrainType, TerrainGenerator& generator, TerrainConfig& config)
{
    auto startTime = std::chrono::steady_clock::now();
    auto terrain = generator.generate(config);
    auto endTime = std::chrono::steady_clock::now();
    auto timeDiffMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << " Terrain Type         : " << terrainType << std::endl;
    std::cout << " Generation Time      : " << timeDiffMs << " ms "<<std::endl;
    std::cout << " Num Points Generated : " << terrain.heights.size()<<std::endl;
    std::cout << " -----------------------"<<std::endl;
}


int main()
{
    TerrainConfig hillConfig;
    TerrainConfig stepsConfig;
    TerrainConfig planeConfig;

    TerrainGenerator generator;

    hillConfig.terrainType =TerrainType::Hills;
    hillConfig.xSize = 10.0;
    hillConfig.ySize = 10.0;
    hillConfig.resolution = 0.005;
    hillConfig.amplitude = 0.2;
    hillConfig.frequency = 0.2;
    hillConfig.roughenss = 0.0;

    planeConfig.terrainType =TerrainType::Plane;
    planeConfig.xSize = 10.0;
    planeConfig.ySize = 10.0;
    planeConfig.resolution = 0.005;


    stepsConfig.terrainType =TerrainType::Steps;
    stepsConfig.xSize = 10.0;
    stepsConfig.ySize = 10.0;
    stepsConfig.resolution = 0.005;
    stepsConfig.stepWidth = 0.1;
    stepsConfig.stepHeight = 0.1;


    testConfig("Hills ", generator, hillConfig);
    testConfig("Steps ", generator, stepsConfig);
    testConfig("Plane ",generator, planeConfig);

}


/*
 Results on i7-10700

 Terrain Type         : Hills
 Generation Time      : 37 ms
 Num Points Generated : 4004001
 -----------------------
 Terrain Type         : Steps
 Generation Time      : 10 ms
 Num Points Generated : 4004001
 -----------------------
 Terrain Type         : Plane
 Generation Time      : 4 ms
 Num Points Generated : 4004001
 -----------------------

 */