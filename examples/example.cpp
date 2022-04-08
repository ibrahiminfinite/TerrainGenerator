

#include <assert.h>
#include <iostream>

#include "TerrainGenerator.h"

int main()
{
    TerrainGenerator generator;
    TerrainConfig config;

    config.xSize = config.ySize = 8.0;
    config.resolution = 0.2;
    config.amplitude = 1.0;
    config.frequency = 1.0;
    config.roughenss = 1.0;
    config.numOctaves = 1;

    auto s = generator.generate(config).heights.size();
    assert(s == 1681); // 41 * 41
    std::cout<< "SIZE : "<<s<<std::endl;

}