// Author : Poh Zhi Ee (github : pohzhiee)
#include "TerrainHelpers.hpp"
#include <iostream>

namespace Terrains
{
struct Point2D
{
    double x;
    double y;
};

bool PointInTriangle(Point2D pt, Point2D v1, Point2D v2, Point2D v3)
{
    double d1, d2, d3;
    bool has_neg, has_pos;
    const auto Sign = [](Point2D p1, Point2D p2, Point2D p3) {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    d1 = Sign(pt, v1, v2);
    d2 = Sign(pt, v2, v3);
    d3 = Sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

Point2D ToGridCoords(double x, double y, const Terrain &terrain)
{
    auto config = terrain.config;

    // Perform conversion from cartesian coordinates to grid coordinates

    // add offset in meters (to compensate centering)
    auto xc = x + config.xSize / 2;
    auto yc = y + config.ySize / 2;

    // scaling
    auto xg = xc / config.resolution;
    auto yg = yc / config.resolution;

    return Point2D{.x = xg, .y = yg};
}

auto GetBoundingTriEdges(const Point2D &point, const Terrain &terrain)
{
    /*
     * For any given point, we want to determine the nearest 3 points of its bounding rectangle
     * If the point itself lies on a line between 2 grid points, then there will be 2 same points out of the 3
     * if the point itself lies on a grid point, then all 3 returned points will be the same
     * Grid point refers to a point with height value in the heightmap grid
     *
     * In the calculations below we assume that the grid is being triangulated this way
     P1----------P3
     |  \         |
     |    \       |
     |      \     |
     |        \   |
     P0----------P2
    Coordinate representation:
           ^ X
           |
     Y<----|
       (z is in direction out of screen towards reader)
     */
    const auto numXsamples = static_cast<int32_t>(terrain.config.xSize / terrain.config.resolution) + 1;
    const auto numYsamples = static_cast<int32_t>(terrain.config.ySize / terrain.config.resolution) + 1;
    Eigen::Map<const Eigen::Matrix<float, -1, -1>> hmap(terrain.heights.data(), numXsamples, numYsamples);
    const auto xg = point.x;
    const auto yg = point.y;
    Point2D oriPoint{.x = xg, .y = yg};
    std::array<const Point2D, 4> nearestPoints{
        Point2D{.x = std::floor(xg), .y = std::ceil(yg)},
        Point2D{.x = std::ceil(xg), .y = std::ceil(yg)},
        Point2D{.x = std::floor(xg), .y = std::floor(yg)},
        Point2D{.x = std::ceil(xg), .y = std::floor(yg)},
    };
    bool inLeftTri = PointInTriangle(oriPoint, nearestPoints[0], nearestPoints[1], nearestPoints[2]);
    bool inRightTri = PointInTriangle(oriPoint, nearestPoints[1], nearestPoints[2], nearestPoints[3]);
    if (!inLeftTri && !inRightTri)
    {
        throw std::runtime_error("Point not in left or right triangle, check code logic");
    }
    auto pointToEigenVec = [hmap](const Point2D &point) {
        return Eigen::Vector3d(point.x, point.y, hmap(static_cast<int>(point.x), static_cast<int>(point.y)));
    };
    if (inLeftTri)
    {
        return std::array<const Eigen::Vector3d, 3>{
            pointToEigenVec(nearestPoints[0]), pointToEigenVec(nearestPoints[1]), pointToEigenVec(nearestPoints[2])};
    }
    return std::array<const Eigen::Vector3d, 3>{pointToEigenVec(nearestPoints[1]), pointToEigenVec(nearestPoints[2]),
                                                pointToEigenVec(nearestPoints[3])};
}

double InterpZ(double x, double y, const std::array<const Eigen::Vector3d, 3> &points)
{
    // Refer to this post:
    // https://math.stackexchange.com/questions/3675166/how-to-handle-degenerate-case-for-interpolating-with-3-point-plane

    // Check if all points are same, if so just return the height
    if (points.at(0).isApprox(points.at(1)) and points.at(0).isApprox(points.at(2)))
    {
        return points.at(0).z();
    }

    // If all 3 points are distinct, we can form a plane and calculate the intersection and return that as height
    if (!points.at(0).isApprox(points.at(1)) and !points.at(0).isApprox(points.at(2)))
    {
        // 3 distinct points, so we can form a plane
        Eigen::Vector3d AB = points[1] - points[0];
        Eigen::Vector3d AC = points[2] - points[0];
        Eigen::Vector3d planeNormal = AB.cross(AC);
        double constant = points.at(0).dot(planeNormal);
        double height = (constant - planeNormal.x() * x - planeNormal.y() * y) / planeNormal.z();
        return height;
    }

    // There are only 2 distinct points, so figure out the 2 distinct points and perform interpolation
    Eigen::Vector3d point1, point2;
    if (points[0].isApprox(points[1]))
    {
        point1 = points[0];
        point2 = points[2];
    }
    else
    {
        point1 = points[0];
        point2 = points[1];
    }
    Eigen::Vector3d direction = (point1 - point2).normalized();
    if (std::abs(direction.x()) < DBL_MIN)
    {
        return (y - point1.y()) / direction.y() * direction.z() + point1.z();
    }
    return (x - point1.x()) / direction.x() * direction.z() + point1.z();
}

double GetHeight(double x, double y, Terrain &terrain)
{
    auto pointGrid = ToGridCoords(x, y, terrain);
    auto boundingEdges = GetBoundingTriEdges(pointGrid, terrain);
    return InterpZ(pointGrid.x, pointGrid.y, boundingEdges);
}

} // namespace Terrains