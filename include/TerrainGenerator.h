
#ifndef TERRAINGENERATOR_TERRAINGENERATOR_H
#define TERRAINGENERATOR_TERRAINGENERATOR_H

#include <vector>
#include <random>

#define UNUSED 143255

enum struct TerrainType
{
    Hills,
    Steps,
    Plane
};


struct TerrainConfig
{
    TerrainType terrainType;

    float xSize, ySize; // in meters
    float resolution; // size of a square in meters

    // To be used with hill terrain
    float roughenss = UNUSED;
    float amplitude = UNUSED;
    float frequency = UNUSED;
    int numOctaves = UNUSED;

    // To be used with steps and stairs
    float stepWidth = UNUSED;
    float stepHeight = UNUSED;
};


struct Terrain
{
    std::vector<float> heights;
    TerrainConfig config;
};

class TerrainGenerator
{

public:

    TerrainGenerator();

    // Returns an instance of Terrain based on provided configuration
    Terrain generate(const TerrainConfig& config);

    Terrain generateHills(const TerrainConfig& config);
    Terrain generateSteps(const TerrainConfig& config);
    Terrain generatePlane(const TerrainConfig& config);


protected:
    std::mt19937 engine_;
    std::uniform_real_distribution<float> uniformDist_;
};

#endif //TERRAINGENERATOR_TERRAINGENERATOR_H
