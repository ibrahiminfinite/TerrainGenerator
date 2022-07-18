#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "PerlinNoise.hpp"
#include <Eigen/Dense>
#include <random>
#include <vector>

enum struct TerrainType
{
    Invalid = 0,
    Hills,
    Steps,
    Stairs,
    Plane
};

struct TerrainConfig
{
    TerrainType terrainType = TerrainType::Invalid;
    int seed = -1;

    double xSize, ySize = -1; // in meters
    double resolution = -1;   // size of a square in meters

    double slope = 0; // slope in degrees
    bool slopeX = true; // slopes about x-axis if true , else slopes about y-axis

    // To be used with hill terrain
    float roughness = -1;
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
};
namespace Terrains
{
class TerrainGenerator
{

  public:
    TerrainGenerator();

    // Returns an instance of Terrain based on provided configuration
    Terrain generate(const TerrainConfig &config);



  protected:
    Terrain generateHills(const TerrainConfig &config);
    Terrain generateSteps(const TerrainConfig &config);
    Terrain generatePlane(const TerrainConfig &config);
    Terrain generateStairs(const TerrainConfig &config);
    void slopeTerrain(Eigen::MatrixXf &heights,
                      const TerrainConfig &config);

    void toStdVec(const Eigen::MatrixXf &heights,
                  const TerrainConfig &config,
                  Terrain &terrain);
    std::uniform_real_distribution<float> uniformDist_;
};
}
#endif // TERRAINGENERATOR_H