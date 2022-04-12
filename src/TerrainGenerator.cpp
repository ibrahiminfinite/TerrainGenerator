
#include "TerrainGenerator.h"
#include "PerlinNoise.h"
#include <math.h>
#include <Eigen/Dense>

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
        case TerrainType::Plane : return generatePlane(config);
                        default :  return Terrain();
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
    // We need (nx + 1) * (ny+1) vertices for a grid of size (nx * ny)
    const size_t nVerticesX = int(config.xSize / config.resolution) + 1;
    const size_t nVerticesY = int(config.ySize / config.resolution) + 1;

    // Calcluate the no: of segments of `stepWidth` that will fit in the terrain size
    int nSegmentsX = int(config.xSize / config.stepWidth);
    int nSegmentsY = int(config.ySize / config.stepWidth);

    // calculate number of vertices needed for each segment
    int VerticesPerSegment = int(config.stepWidth / config.resolution);

    // Using eigen to avoid explicit looping to set heights in a std::vector<>
    // The values for full square can be set using block
    std::vector<float> heights;
    heights.resize(nVerticesX * nVerticesY);
    // Using map instead of Matrix eliminates the need to convert from eigen matrix
    // to std::vector as Map uses in place operations
    Eigen::Map<Eigen::Matrix<float,  -1, -1>> hmap(heights.data(), nVerticesX, nVerticesY);
    hmap.setZero();


    // Loop through the squares with (VerticesPerSegment * VerticesPerSegment) vertices each
    float height;
    for(int i = 0; i < nSegmentsX; ++i)
    {
        for(int j = 0; j <nSegmentsY; ++j)
        {
            // set random height in range (0, 0.5)
            height = uniformDist_(engine_) * config.stepHeight;
            hmap.block(i * VerticesPerSegment, j * VerticesPerSegment,
                          VerticesPerSegment, VerticesPerSegment).setConstant(height);
        }
    }

    Terrain terrain;
    terrain.heights = heights;
    terrain.config = config;

    return terrain;
}