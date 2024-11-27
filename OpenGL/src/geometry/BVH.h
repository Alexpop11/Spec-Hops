#pragma once
#include <vector>
#include <memory>
#include "AABB.h"
#include <glm/glm.hpp>

struct LineSegment {
   glm::vec2 start;
   glm::vec2 end;
   AABB      bounds;

   LineSegment(const glm::vec2& s, const glm::vec2& e)
      : start(s)
      , end(e)
      , bounds(s, e) {}
};

class BVHNode {
public:
   AABB                     bounds;
   std::unique_ptr<BVHNode> left;
   std::unique_ptr<BVHNode> right;
   std::vector<LineSegment> segments;
   bool                     isLeaf;

   static constexpr size_t MAX_SEGMENTS_PER_LEAF = 4;

   BVHNode()
      : isLeaf(true) {}

   // Returns true if the given line segment intersects with any segment in the BVH
   bool intersectsLine(const glm::vec2& start, const glm::vec2& end) const;

   // Returns the first intersection point of a ray with any segment in the BVH
   std::optional<glm::vec2> intersectRay(const glm::vec2& origin, const glm::vec2& direction) const;

   // Builds a BVH from a vector of line segments
   static std::unique_ptr<BVHNode> build(std::vector<LineSegment>& segments, int depth = 0);
};
