#ifndef BVH_H
#define BVH_H

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <limits>
#include <vector>
#include <optional>

struct Ray {
   glm::vec2 origin;
   glm::vec2 direction;
};

struct AABB {
   glm::vec2 min;
   glm::vec2 max;
};

struct Segment {
   glm::vec2 start;
   glm::vec2 end;
};

struct BVHNode {
   AABB   aabb;
   size_t left;
   size_t right;
   size_t segment_start;
   size_t segment_end;

   bool is_leaf() const;
};

struct BVH {
   std::vector<BVHNode> nodes;
   std::vector<Segment> segments;

   static BVH build(std::vector<Segment> segments);

   size_t build_recursive(size_t segment_start, size_t segment_end);

   AABB compute_aabb(size_t segment_start, size_t segment_end) const;

   size_t partition_segments(size_t segment_start, size_t segment_end);

   /// Find the closest intersection point between a ray and any segment in the BVH.
   std::optional<std::pair<glm::vec2, const Segment*>> ray_intersect(const Ray& ray) const;

   std::optional<std::tuple<glm::vec2, float, const Segment*>>
   ray_intersect_node(const Ray& ray, size_t node_idx,
                      std::optional<std::tuple<glm::vec2, float, const Segment*>> closest) const;

   /// Find the closest intersection point between a segment and any segment in the BVH.
   std::optional<std::pair<glm::vec2, const Segment*>> segment_intersect(const Segment& segment) const;

   std::optional<std::tuple<glm::vec2, float, const Segment*>>
   segment_intersect_node(const Segment& segment, size_t node_idx,
                          std::optional<std::tuple<glm::vec2, float, const Segment*>> closest) const;
};

// Utility functions for intersection tests

float cross(const glm::vec2& v, const glm::vec2& w);

extern const float EPSILON;

/// Compute the intersection point between a ray and a segment.
/// Returns (intersection_point, t) where t is the parameter along the ray.
std::optional<std::pair<glm::vec2, float>> intersect_ray_segment(const Ray& ray, const Segment& segment);

/// Compute the intersection point between two segments.
/// Returns (intersection_point, t) where t is the parameter along the first segment.
std::optional<std::pair<glm::vec2, float>> intersect_segment_segment(const Segment& s1, const Segment& s2);

/// Check if a ray intersects an AABB.
/// Returns Some((tmin, tmax)) if there is an intersection, None otherwise.
std::optional<std::pair<float, float>> intersect_ray_aabb(const Ray& ray, const AABB& aabb);

/// Check if a segment intersects an AABB.
bool intersect_segment_aabb(const Segment& segment, const AABB& aabb);

#endif // BVH_H
