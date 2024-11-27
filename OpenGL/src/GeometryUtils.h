#pragma once

#include <vector>
#include <functional>
#include <optional>
#include "clipper2/clipper.h"
#include <glm/glm.hpp>
#include "earcut.hpp"
#include "geometry/BVH.h"

namespace GeometryUtils {
using namespace Clipper2Lib;

/**
 * @brief Finds the union of multiple polygons.
 *
 * @param polygons A vector of polygons, where each polygon is represented as a vector of glm::vec2 points.
 * @param output A PolyTreeD object to store the resulting union.
 * @return true if the union operation was successful, false otherwise.
 */
bool findPolygonUnion(const std::vector<std::vector<glm::vec2>>& polygons, PolyTreeD& output);

/**
 * @brief Flattens a hierarchical PolyPathD into a simple PathsD structure.
 *
 * @param polyPath The root PolyPathD to flatten.
 * @return PathsD A flattened PathsD containing all polygons from the hierarchy.
 */
PathsD FlattenPolyPathD(const PolyPathD& polyPath);

/**
 * @brief Computes the visibility polygon from a given position and obstacles.
 *
 * @param position The player's position as glm::vec2.
 * @param obstacles The obstacles represented as PathsD (vector of paths).
 * @return PathD The visibility polygon as a vector of points.
 */
PathD ComputeVisibilityPolygon(const glm::vec2& position, const PathsD& obstacles, const BVHNode& bvh);

/**
 * @brief Computes the intersection point between a ray and a line segment.
 *
 * @param ray_origin The origin point of the ray.
 * @param dx The x-component of the ray's direction vector.
 * @param dy The y-component of the ray's direction vector.
 * @param a The first endpoint of the line segment.
 * @param b The second endpoint of the line segment.
 * @return std::optional<glm::vec2> containing the intersection point if it exists.
 */
std::optional<glm::vec2> RaySegmentIntersect(const glm::vec2& ray_origin, double dx, double dy, const glm::vec2& a,
                                             const glm::vec2& b);

/**
 * @brief Computes the intersection point between two line segments.
 *
 * @param line1_start The starting point of the first line segment.
 * @param line1_end The ending point of the first line segment.
 * @param line2_start The starting point of the second line segment.
 * @param line2_end The ending point of the second line segment.
 * @return std::optional<glm::vec2> containing the intersection point if it exists.
 */
std::optional<glm::vec2> LineSegmentIntersect(const glm::vec2& line1_start, const glm::vec2& line1_end,
                                              const glm::vec2& line2_start, const glm::vec2& line2_end);

float length2(const glm::vec2& a, const glm::vec2& b);
float length2(const glm::vec2& a);
} // namespace GeometryUtils


namespace mapbox {
namespace util {

template <>
struct nth<0, Clipper2Lib::PointD> {
   inline static auto get(const Clipper2Lib::PointD& t) { return t.x; };
};

template <>
struct nth<1, Clipper2Lib::PointD> {
   inline static auto get(const Clipper2Lib::PointD& t) { return t.y; };
};

} // namespace util
} // namespace mapbox
