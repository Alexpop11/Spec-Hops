#include "BVH.h"
#include "../GeometryUtils.h"
#include <algorithm>
#include "../rendering/Renderer.h"

std::optional<glm::vec2> BVHNode::intersectsLine(const glm::vec2& start, const glm::vec2& end) const {
   // Create AABB for the query line segment
   AABB queryBounds(start, end);

   // Early exit if no overlap with node bounds
   if (!bounds.intersects(queryBounds)) {
      return std::nullopt;
   }

   // Check segments in leaf node
   if (isLeaf) {
      std::optional<glm::vec2> closest;
      float                    closest_distance = std::numeric_limits<float>::max();
      float                    minDist          = 0.01f;

      for (const auto& segment : segments) {
         if (auto intersection = GeometryUtils::LineSegmentIntersect(start, end, segment.start, segment.end)) {
            auto current_distance = GeometryUtils::length2(*intersection - start);
            if (current_distance > minDist && current_distance < closest_distance) {
               closest          = intersection;
               closest_distance = current_distance;
            }
         }
      }
      return closest;
   }

   // Recursively check children
   auto leftHit  = left ? left->intersectsLine(start, end) : std::nullopt;
   auto rightHit = right ? right->intersectsLine(start, end) : std::nullopt;

   if (leftHit.has_value() && rightHit.has_value()) {
      float leftDist  = GeometryUtils::length2(*leftHit - start);
      float rightDist = GeometryUtils::length2(*rightHit - start);
      auto  hit       = leftDist < rightDist ? leftHit : rightHit;
      return hit;
   }
   auto hit = leftHit ? leftHit : rightHit;
   return hit;
}

void BVHNode::debugDraw(const glm::vec4& color) const {
   // Draw this node's AABB
   glm::vec2 topRight(bounds.max.x, bounds.min.y);
   glm::vec2 bottomLeft(bounds.min.x, bounds.max.y);

   Renderer::DebugLine(bounds.min, topRight, color);
   Renderer::DebugLine(topRight, bounds.max, color);
   Renderer::DebugLine(bounds.max, bottomLeft, color);
   Renderer::DebugLine(bottomLeft, bounds.min, color);

   // Recursively draw children with slightly different colors
   if (!isLeaf) {
      if (left) {
         left->debugDraw(glm::vec4(color.r, color.g * 0.8f, color.b, color.a));
      }
      if (right) {
         right->debugDraw(glm::vec4(color.r * 0.8f, color.g, color.b, color.a));
      }
   } else {
      for (const auto& segment : segments) {
         Renderer::DebugLine(segment.start, segment.end, color);
      }
   }
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
      float                    closest_distance = std::numeric_limits<float>::max();

      float minDist = 0.01f;

      for (const auto& segment : segments) {
         if (auto hit =
                GeometryUtils::RaySegmentIntersect(origin, direction.x, direction.y, segment.start, segment.end)) {
            float dist = GeometryUtils::length2(*hit - origin);
            if (dist > minDist && dist < closest_distance) {
               closest          = hit;
               closest_distance = dist;
            }
         }
      }
      return closest;
   }

   // Check children
   auto leftHit  = left ? left->intersectRay(origin, direction) : std::nullopt;
   auto rightHit = right ? right->intersectRay(origin, direction) : std::nullopt;

   // Return closest intersection
   if (leftHit.has_value() && rightHit.has_value()) {
      float leftDist  = GeometryUtils::length2(*leftHit - origin);
      float rightDist = GeometryUtils::length2(*rightHit - origin);
      auto  hit       = leftDist < rightDist ? leftHit : rightHit;
      return hit;
   }
   auto hit = leftHit ? leftHit : rightHit;
   return hit;
}

// TODO: This is a naive BVH builder that does not attempt to optimize the tree.
// The tree is actually pretty bad because the bounding volumes overlap lol.
// It should probably use a SAH or some other more sophisticated method.
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
