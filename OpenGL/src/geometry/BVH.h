#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <limits>
#include <vector>
#include <optional>
#include <cstdint>

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

struct BvhNode {
   uint32_t leftTypeCount; // leftType :1, leftCount :31
   uint32_t leftOffset;

   uint32_t rightTypeCount; // rightType :1, rightCount :31
   uint32_t rightOffset;

   glm::vec4 leftBBox; // (min.x, min.y, max.x, max.y)
   glm::vec4 rightBBox;

   // Helper methods for left child
   uint32_t getLeftType() const { return leftTypeCount >> 31; }
   uint32_t getLeftCount() const { return leftTypeCount & 0x7FFFFFFF; }
   void     setLeft(uint32_t type, uint32_t count) { leftTypeCount = (type << 31) | (count & 0x7FFFFFFF); }

   // Helper methods for right child
   uint32_t getRightType() const { return rightTypeCount >> 31; }
   uint32_t getRightCount() const { return rightTypeCount & 0x7FFFFFFF; }
   void     setRight(uint32_t type, uint32_t count) { rightTypeCount = (type << 31) | (count & 0x7FFFFFFF); }
};

struct BVH {
   std::vector<BvhNode> nodes;
   std::vector<Segment> segments;

   static BVH build(std::vector<Segment> segments);

   size_t build_recursive(size_t segment_start, size_t segment_end);

   AABB compute_aabb(size_t segment_start, size_t segment_end) const;

   size_t partition_segments(size_t segment_start, size_t segment_end);

   glm::vec4 getBoundingBoxOfNode(const BvhNode& node) const;

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
