
#ifndef TERRAINGENERATOR_TERRAINGENERATOR_H
#define TERRAINGENERATOR_TERRAINGENERATOR_H

#include <vector>
#include <random>
#include <dart/dart.hpp>


typedef dart::dynamics::SimpleFramePtr VisualPtr;
typedef dart::dynamics::HeightmapShape<float> HeightmapShape;
typedef dart::dynamics::SkeletonPtr CollisionPtr;

/************************** NOISE ********************************/
class PerlinNoise {

public:

    PerlinNoise();

    float noise(float x, float y, float z);

private:

    std::vector<int> p;

    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(int hash, float x, float y, float z);
};

/************************** TERRAIN  ********************************/

enum struct TerrainType
{
    Invalid=0,
    Hills,
    Steps,
    Plane
};

struct TerrainConfig
{
    TerrainType terrainType = TerrainType::Invalid;

    float xSize, ySize = -1; // in meters
    float resolution = -1; // size of a square in meters

    // To be used with hill terrain
    float roughenss = -1;
    float amplitude = -1;
    float frequency = -1;
    int numOctaves = -1;

    // To be used with steps and stairs
    float stepWidth = -1;
    float stepHeight = -1;
};

struct Terrain
{
    std::vector<float> heights;
    TerrainConfig config;
    VisualPtr visual;
    CollisionPtr collision;
};

class TerrainGenerator
{

public:

    TerrainGenerator();

    // Returns an instance of Terrain based on provided configuration
    Terrain generate(const TerrainConfig& config);

private:

    Terrain terrain_;
    std::shared_ptr<HeightmapShape> terrainShape_;
    std::mt19937 engine_;
    std::uniform_real_distribution<float> uniformDist_;

    Terrain generateHills(const TerrainConfig& config);
    Terrain generateSteps(const TerrainConfig& config);
    Terrain generatePlane(const TerrainConfig& config);

    void createHeightmapShape(const TerrainConfig& config);
    VisualPtr createHeightmapVisual(const TerrainConfig& config);
    CollisionPtr createHeightMapCollision(const TerrainConfig& tConfig);


};

#endif //TERRAINGENERATOR_TERRAINGENERATOR_H
