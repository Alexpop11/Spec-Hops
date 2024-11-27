#include "BVH.h"
#include "../GeometryUtils.h"
#include <algorithm>

bool BVHNode::intersectsLine(const glm::vec2& start, const glm::vec2& end) const {
   // Create AABB for the query line segment
   AABB queryBounds(start, end);

   // Early exit if no overlap with node bounds
   if (!bounds.intersects(queryBounds)) {
      return false;
   }

   // Check segments in leaf node
   if (isLeaf) {
      for (const auto& segment : segments) {
         if (auto intersection = GeometryUtils::LineSegmentIntersect(start, end, segment.start, segment.end)) {
            return true;
         }
      }
      return false;
   }

   // Recursively check children
   return (left && left->intersectsLine(start, end)) || (right && right->intersectsLine(start, end));
}

std::optional<glm::vec2> BVHNode::intersectRay(const glm::vec2& origin, const glm::vec2& direction) const {
   // Early exit if no overlap with node bounds
   if (auto intersection = GeometryUtils::RaySegmentIntersect(origin, direction.x, direction.y, bounds.min,
                                                              glm::vec2(bounds.max.x, bounds.min.y))) {
      // Continue checking
   } else if (auto intersection = GeometryUtils::RaySegmentIntersect(
                 origin, direction.x, direction.y, glm::vec2(bounds.max.x, bounds.min.y), bounds.max)) {
      // Continue checking
   } else if (auto intersection = GeometryUtils::RaySegmentIntersect(origin, direction.x, direction.y, bounds.max,
                                                                     glm::vec2(bounds.min.x, bounds.max.y))) {
      // Continue checking
   } else if (auto intersection = GeometryUtils::RaySegmentIntersect(
                 origin, direction.x, direction.y, glm::vec2(bounds.min.x, bounds.max.y), bounds.min)) {
      // Continue checking
   } else {
      return std::nullopt; // Ray doesn't intersect bounds
   }

   if (isLeaf) {
      std::optional<glm::vec2> closest;
      float                    minDist = std::numeric_limits<float>::max();

      for (const auto& segment : segments) {
         if (auto hit =
                GeometryUtils::RaySegmentIntersect(origin, direction.x, direction.y, segment.start, segment.end)) {
            float dist = GeometryUtils::length2(*hit - origin);
            if (!closest || dist < minDist) {
               closest = hit;
               minDist = dist;
            }
         }
      }
      return closest;
   }

   // Check children
   auto leftHit  = left ? left->intersectRay(origin, direction) : std::nullopt;
   auto rightHit = right ? right->intersectRay(origin, direction) : std::nullopt;

   // Return closest intersection
   if (leftHit && rightHit) {
      float leftDist  = GeometryUtils::length2(*leftHit - origin);
      float rightDist = GeometryUtils::length2(*rightHit - origin);
      return leftDist < rightDist ? leftHit : rightHit;
   }
   return leftHit ? leftHit : rightHit;
}

std::unique_ptr<BVHNode> BVHNode::build(std::vector<LineSegment>& segments, int depth) {
   auto node = std::make_unique<BVHNode>();

   // Compute bounds
   for (const auto& segment : segments) {
      node->bounds = node->bounds.merge(segment.bounds);
   }

   // If few enough segments, make a leaf
   if (segments.size() <= MAX_SEGMENTS_PER_LEAF || depth > 20) {
      node->segments = std::move(segments);
      return node;
   }

   // Split along longest axis
   glm::vec2 extent = node->bounds.max - node->bounds.min;
   int       axis   = extent.x > extent.y ? 0 : 1;

   // Sort segments by centroid
   std::sort(segments.begin(), segments.end(), [axis](const LineSegment& a, const LineSegment& b) {
      glm::vec2 centroidA = (a.start + a.end) * 0.5f;
      glm::vec2 centroidB = (b.start + b.end) * 0.5f;
      return centroidA[axis] < centroidB[axis];
   });

   // Split segments into two groups
   size_t                   mid = segments.size() / 2;
   std::vector<LineSegment> leftSegments(segments.begin(), segments.begin() + mid);
   std::vector<LineSegment> rightSegments(segments.begin() + mid, segments.end());

   // Recursively build children
   node->isLeaf = false;
   node->left   = build(leftSegments, depth + 1);
   node->right  = build(rightSegments, depth + 1);

   return node;
}
