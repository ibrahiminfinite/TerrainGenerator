
#include "TerrainGenerator.h"
#include "PerlinNoise.h"
#include <math.h>

TerrainGenerator::TerrainGenerator()
{
    std::default_random_engine rd;
    engine_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0,1.0);
}

Terrain TerrainGenerator::generate(const TerrainConfig& config)
{

    switch (config.terrainType)
    {
        case TerrainType::Hills : return generateHills(config);
        case TerrainType::Steps : return generateSteps(config);
                        default : return generatePlane(config);
    }
}


Terrain TerrainGenerator::generateHills(const TerrainConfig& config)
{
    // We need (nx + 1) * (ny+1) vertices for a grid of size (nx * ny)
    size_t numVerticesX = (config.xSize / config.resolution) + 1;
    size_t numVerticesY = (config.ySize / config.resolution) + 1;


    PerlinNoise noiseGenerator;

    Terrain terrain;
    float amp, freq, height{0};

    for(int i = 0; i < numVerticesX; ++i)
    {
        for(int j = 0; j < numVerticesY; ++j)
        {

            height = 0;
            // generate perlin noise based terrain
            for (int k = 0; k < config.numOctaves; ++k)
            {
                amp = config.amplitude / pow(2, k);
                freq = config.frequency * pow(2,k);


                height += amp * noiseGenerator.noise(i * freq, j * freq, 0.1);
            }

            // TODO : add roughness here
            height += config.roughenss * uniformDist_(engine_);
            terrain.heights.emplace_back(height);
        }
    }

    terrain.config = config;

    return terrain;
}


Terrain TerrainGenerator::generatePlane(const TerrainConfig& config)
{
    // We need (nx + 1) * (ny+1) vertices for a grid of size (nx * ny)
    size_t numVerticesX = (config.xSize / config.resolution) + 1;
    size_t numVerticesY = (config.ySize / config.resolution) + 1;

    Terrain terrain;

    for(int i = 0; i < numVerticesX * numVerticesY; ++i)
    {
        terrain.heights.emplace_back(0.0);
    }
    terrain.config = config;

    return  terrain;
}


Terrain TerrainGenerator::generateSteps(const TerrainConfig &config)
{
    return Terrain();
}