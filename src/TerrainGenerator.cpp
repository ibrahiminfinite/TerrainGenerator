
#include "TerrainGenerator.h"
#include <math.h>
#include <Eigen/Dense>


/************************** NOISE ********************************/

PerlinNoise::PerlinNoise() {

    p = {
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
            8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
            35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
            134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
            55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
            18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
            250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
            189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
            43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
            97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
            107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

    p.insert(p.end(), p.begin(), p.end());
}

float PerlinNoise::noise(float x, float y, float z) {
    // Find the unit cube that contains the point
    int X = (int) floor(x) & 255;
    int Y = (int) floor(y) & 255;
    int Z = (int) floor(z) & 255;

    // Find relative x, y,z of point in cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute fade curves for each of x, y, z
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add interpolated results from 8 corners of cube
    float res = lerp(w,
                     lerp(v,
                          lerp(u, grad(p[AA], x, y, z), grad(p[BA], x-1, y, z)),
                          lerp(u, grad(p[AB], x, y-1, z), grad(p[BB], x-1, y-1, z))),
                     lerp(v,
                          lerp(u, grad(p[AA+1], x, y, z-1), grad(p[BA+1], x-1, y, z-1)),
                          lerp(u, grad(p[AB+1], x, y-1, z-1),grad(p[BB+1], x-1, y-1, z-1))));
    return (res + 1.0)/2.0;
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    // Convert lower 4 bits of hash into 12 gradient directions
    float u = h < 8 ? x : y,
            v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


/************************** TERRAIN  ********************************/

TerrainGenerator::TerrainGenerator()
{
    std::default_random_engine rd;
    engine_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0,1.0);
    terrainShape_ = std::make_shared<HeightmapShape>();
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
            terrain_.heights.emplace_back(height);
        }
    }

    terrain_.config = config;

    createHeightmapShape(config);
    terrain_.visual = createHeightmapVisual(config);
    terrain_.collision = createHeightMapCollision(config);

    return terrain_;
}

Terrain TerrainGenerator::generatePlane(const TerrainConfig& config)
{
    // We need (nx + 1) * (ny+1) vertices for a grid of size (nx * ny)
    size_t numVerticesX = (config.xSize / config.resolution) + 1;
    size_t numVerticesY = (config.ySize / config.resolution) + 1;

    for(int i = 0; i < numVerticesX * numVerticesY; ++i)
    {
        terrain_.heights.emplace_back(0.0);
    }
    terrain_.config = config;

    createHeightmapShape(config);
    terrain_.visual = createHeightmapVisual(config);
    terrain_.collision = createHeightMapCollision(config);

    return  terrain_;
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


    terrain_.heights = heights;
    terrain_.config = config;

    createHeightmapShape(config);
    terrain_.visual = createHeightmapVisual(config);
    terrain_.collision = createHeightMapCollision(config);

    return terrain_;
}


void TerrainGenerator::createHeightmapShape(const TerrainConfig& config)
{
    if(config.terrainType == TerrainType::Invalid)
    {
        dterr << "Invalid Terrain Type"<< std::endl;
    }

    using Vector3 = Eigen::Matrix<float, 3, 1>;

    auto scale = Vector3(config.resolution,config.resolution, 1);

    terrainShape_->setScale(scale);
    auto xs = int(config.xSize / config.resolution) + 1;
    auto ys = int(config.ySize / config.resolution) + 1;
    std::cout<<" XS : "<<xs << " YS : "<<ys<<" HS : "<<terrain_.heights.size()<<std::endl;

    terrainShape_->setHeightField(xs,
                              ys,
                              terrain_.heights);
}

// For visual
VisualPtr TerrainGenerator::createHeightmapVisual(const TerrainConfig& config)
{
    auto terrainFrame = dart::dynamics::SimpleFrame::createShared(dart::dynamics::Frame::World());
    auto tf = terrainFrame->getRelativeTransform();
    tf.translation()[0] = -static_cast<double>(config.xSize) / 2.0;
    tf.translation()[1] = +static_cast<double>(config.ySize) / 2.0;
    terrainFrame->setRelativeTransform(tf);

    terrainFrame->createVisualAspect();

    //TODO :set shape
    terrainFrame->setShape(terrainShape_);
    return terrainFrame;
}


CollisionPtr TerrainGenerator::createHeightMapCollision(const TerrainConfig& tConfig) {

    dart::dynamics::SkeletonPtr terrainSkel =
            dart::dynamics::Skeleton::create();

    dart::dynamics::BodyNodePtr terrainBody
            = terrainSkel->createJointAndBodyNodePair<
                    dart::dynamics::WeldJoint>(nullptr).second;

    Eigen::Isometry3d tf_trans = Eigen::Isometry3d::Identity();
    tf_trans.translation() = Eigen::Vector3d{0.0, 0.0, 0.0};
    terrainBody->getParentJoint()->setTransformFromParentBodyNode(tf_trans);

    dart::dynamics::ShapeNode *shapeNode = terrainBody->createShapeNodeWith<
            dart::dynamics::CollisionAspect,
            dart::dynamics::DynamicsAspect>(terrainShape_);

    shapeNode->setRelativeTransform(tf_trans);

    return  terrainSkel;
}
