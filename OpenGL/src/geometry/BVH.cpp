#include "BVH.h"
#include <algorithm>
#include <cmath>

const float  EPSILON       = 1e-8f;
const size_t MAX_LEAF_SIZE = 1; // Adjust as needed

float cross(const glm::vec2& v, const glm::vec2& w) {
   return v.x * w.y - v.y * w.x;
}

BVH BVH::build(std::vector<Segment> segments) {
   BVH bvh;
   if (segments.empty()) {
      bvh.segments = std::move(segments);
      return bvh;
   }
   bvh.segments = std::move(segments);
   // Build the tree recursively starting with all segments
   bvh.build_recursive(0, bvh.segments.size());
   return bvh;
}

glm::vec4 BVH::getBoundingBoxOfNode(const BvhNode& node) const {
   float min_x = node.leftBBox.x;
   float min_y = node.leftBBox.y;
   float max_x = node.leftBBox.z;
   float max_y = node.leftBBox.w;

   // Check if right child has a valid bounding box
   if (node.rightBBox != glm::vec4(0.0f)) {
      min_x = std::min(min_x, node.rightBBox.x);
      min_y = std::min(min_y, node.rightBBox.y);
      max_x = std::max(max_x, node.rightBBox.z);
      max_y = std::max(max_y, node.rightBBox.w);
   }

   return glm::vec4(min_x, min_y, max_x, max_y);
}

size_t BVH::build_recursive(size_t segment_start, size_t segment_end) {
   // Calculate AABB for current node
   AABB aabb = compute_aabb(segment_start, segment_end);

   // Create leaf node if we have few enough segments
   if (segment_end - segment_start <= MAX_LEAF_SIZE) {
      BvhNode node;

      // Leaf node - set left child as line bucket
      uint32_t leftType  = 1; // 1 indicates a line bucket
      uint32_t leftCount = static_cast<uint32_t>(segment_end - segment_start);
      node.setLeft(leftType, leftCount);
      node.leftOffset = static_cast<uint32_t>(segment_start); // Index into segments array

      // Set leftBBox
      node.leftBBox = glm::vec4(aabb.min, aabb.max);

      // No right child for leaf node
      node.setRight(0, 0); // Right child is unused
      node.rightOffset = 0;
      node.rightBBox   = glm::vec4(0.0f); // Empty bounding box

      size_t node_idx = nodes.size();
      nodes.push_back(node);
      return node_idx;
   }

   // Partition segments
   size_t mid = partition_segments(segment_start, segment_end);

   // Recursively build left and right children
   size_t leftChildIndex  = build_recursive(segment_start, mid);
   size_t rightChildIndex = build_recursive(mid, segment_end);

   // Compute bounding boxes of children
   glm::vec4 leftChildBBox  = getBoundingBoxOfNode(nodes[leftChildIndex]);
   glm::vec4 rightChildBBox = getBoundingBoxOfNode(nodes[rightChildIndex]);

   // Create internal node
   BvhNode node;

   // Left child
   node.setLeft(0, 0); // 0 indicates a node
   node.leftOffset = static_cast<uint32_t>(leftChildIndex);
   node.leftBBox   = leftChildBBox;

   // Right child
   node.setRight(0, 0); // 0 indicates a node
   node.rightOffset = static_cast<uint32_t>(rightChildIndex);
   node.rightBBox   = rightChildBBox;

   size_t node_idx = nodes.size();
   nodes.push_back(node);
   return node_idx;
}

AABB BVH::compute_aabb(size_t segment_start, size_t segment_end) const {
   float min_x = std::numeric_limits<float>::infinity();
   float min_y = std::numeric_limits<float>::infinity();
   float max_x = -std::numeric_limits<float>::infinity();
   float max_y = -std::numeric_limits<float>::infinity();

   for (size_t i = segment_start; i < segment_end; ++i) {
      const Segment& segment = segments[i];
      min_x                  = std::min({min_x, segment.start.x, segment.end.x});
      min_y                  = std::min({min_y, segment.start.y, segment.end.y});
      max_x                  = std::max({max_x, segment.start.x, segment.end.x});
      max_y                  = std::max({max_y, segment.start.y, segment.end.y});
   }

   return AABB{
      glm::vec2{min_x, min_y},
      glm::vec2{max_x, max_y}
   };
}

size_t BVH::partition_segments(size_t segment_start, size_t segment_end) {
   AABB  aabb   = compute_aabb(segment_start, segment_end);
   float width  = aabb.max.x - aabb.min.x;
   float height = aabb.max.y - aabb.min.y;

   // Choose axis with the largest spread
   bool axis_vertical = height > width;

   // Lambda to compute the centroid of a segment
   auto centroid = [axis_vertical](const Segment& segment) {
      if (axis_vertical) {
         return (segment.start.y + segment.end.y) / 2.0f;
      } else {
         return (segment.start.x + segment.end.x) / 2.0f;
      }
   };

   // Get mutable iterator range for the segments
   auto segments_begin  = segments.begin() + segment_start;
   auto segments_end_it = segments.begin() + segment_end;

   // Sort the segments in place based on their centroids
   std::sort(segments_begin, segments_end_it,
             [centroid](const Segment& a, const Segment& b) { return centroid(a) < centroid(b); });

   // Return the midpoint index for splitting
   return (segment_start + segment_end) / 2;
}

std::optional<std::pair<glm::vec2, const Segment*>> BVH::ray_intersect(const Ray& ray) const {
   if (nodes.empty()) {
      return std::nullopt;
   }
   auto result = ray_intersect_node(ray, nodes.size() - 1, std::nullopt); // Start from root node
   if (result.has_value()) {
      auto [p, t, segment_ptr] = result.value();
      return std::make_optional(std::make_pair(p, segment_ptr));
   } else {
      return std::nullopt;
   }
}

std::optional<std::tuple<glm::vec2, float, const Segment*>>
BVH::ray_intersect_node(const Ray& ray, size_t node_idx,
                        std::optional<std::tuple<glm::vec2, float, const Segment*>> closest) const {
   const BvhNode& node = nodes[node_idx];

   // Check intersection with left child bounding box
   AABB leftAABB{glm::vec2(node.leftBBox.x, node.leftBBox.y), glm::vec2(node.leftBBox.z, node.leftBBox.w)};
   auto left_aabb_hit = intersect_ray_aabb(ray, leftAABB);

   if (left_aabb_hit.has_value()) {
      uint32_t leftType = node.getLeftType();
      if (leftType == 0) {
         // Left child is a node
         size_t childIdx = node.leftOffset;
         closest         = ray_intersect_node(ray, childIdx, closest);
      } else {
         // Left child is a line bucket (leaf node)
         uint32_t count  = node.getLeftCount();
         uint32_t offset = node.leftOffset;
         for (uint32_t i = 0; i < count; ++i) {
            const Segment& segment      = segments[offset + i];
            auto           intersection = intersect_ray_segment(ray, segment);
            if (intersection.has_value()) {
               glm::vec2 p = intersection->first;
               float     t = intersection->second;
               if (t >= 0.0f) {
                  if (!closest.has_value() || t < std::get<1>(closest.value())) {
                     closest = std::make_optional(std::make_tuple(p, t, &segment));
                  }
               }
            }
         }
      }
   }

   // Check intersection with right child bounding box
   if (node.rightBBox != glm::vec4(0.0f)) { // Check if right child exists
      AABB rightAABB{glm::vec2(node.rightBBox.x, node.rightBBox.y), glm::vec2(node.rightBBox.z, node.rightBBox.w)};
      auto right_aabb_hit = intersect_ray_aabb(ray, rightAABB);

      if (right_aabb_hit.has_value()) {
         uint32_t rightType = node.getRightType();
         if (rightType == 0) {
            // Right child is a node
            size_t childIdx = node.rightOffset;
            closest         = ray_intersect_node(ray, childIdx, closest);
         } else {
            // Right child is a line bucket (leaf node)
            uint32_t count  = node.getRightCount();
            uint32_t offset = node.rightOffset;
            for (uint32_t i = 0; i < count; ++i) {
               const Segment& segment      = segments[offset + i];
               auto           intersection = intersect_ray_segment(ray, segment);
               if (intersection.has_value()) {
                  glm::vec2 p = intersection->first;
                  float     t = intersection->second;
                  if (t >= 0.0f) {
                     if (!closest.has_value() || t < std::get<1>(closest.value())) {
                        closest = std::make_optional(std::make_tuple(p, t, &segment));
                     }
                  }
               }
            }
         }
      }
   }

   return closest;
}

std::optional<std::pair<glm::vec2, const Segment*>> BVH::segment_intersect(const Segment& segment) const {
   if (nodes.empty()) {
      return std::nullopt;
   }
   auto result = segment_intersect_node(segment, nodes.size() - 1, std::nullopt); // Start from root node
   if (result.has_value()) {
      auto [p, t, segment_ptr] = result.value();
      return std::make_optional(std::make_pair(p, segment_ptr));
   } else {
      return std::nullopt;
   }
}

std::optional<std::tuple<glm::vec2, float, const Segment*>>
BVH::segment_intersect_node(const Segment& segment, size_t node_idx,
                            std::optional<std::tuple<glm::vec2, float, const Segment*>> closest) const {
   const BvhNode& node = nodes[node_idx];

   // Check intersection with left child bounding box
   AABB leftAABB{glm::vec2(node.leftBBox.x, node.leftBBox.y), glm::vec2(node.leftBBox.z, node.leftBBox.w)};
   if (intersect_segment_aabb(segment, leftAABB)) {
      uint32_t leftType = node.getLeftType();
      if (leftType == 0) {
         // Left child is a node
         size_t childIdx = node.leftOffset;
         closest         = segment_intersect_node(segment, childIdx, closest);
      } else {
         // Left child is a line bucket (leaf node)
         uint32_t count  = node.getLeftCount();
         uint32_t offset = node.leftOffset;
         for (uint32_t i = 0; i < count; ++i) {
            const Segment& other_segment = segments[offset + i];
            auto           intersection  = intersect_segment_segment(segment, other_segment);
            if (intersection.has_value()) {
               glm::vec2 p = intersection->first;
               float     t = intersection->second;
               if (t >= 0.0f && t <= 1.0f) {
                  if (!closest.has_value() || t < std::get<1>(closest.value())) {
                     closest = std::make_optional(std::make_tuple(p, t, &other_segment));
                  }
               }
            }
         }
      }
   }

   // Check intersection with right child bounding box
   if (node.rightBBox != glm::vec4(0.0f)) { // Check if right child exists
      AABB rightAABB{glm::vec2(node.rightBBox.x, node.rightBBox.y), glm::vec2(node.rightBBox.z, node.rightBBox.w)};
      if (intersect_segment_aabb(segment, rightAABB)) {
         uint32_t rightType = node.getRightType();
         if (rightType == 0) {
            // Right child is a node
            size_t childIdx = node.rightOffset;
            closest         = segment_intersect_node(segment, childIdx, closest);
         } else {
            // Right child is a line bucket (leaf node)
            uint32_t count  = node.getRightCount();
            uint32_t offset = node.rightOffset;
            for (uint32_t i = 0; i < count; ++i) {
               const Segment& other_segment = segments[offset + i];
               auto           intersection  = intersect_segment_segment(segment, other_segment);
               if (intersection.has_value()) {
                  glm::vec2 p = intersection->first;
                  float     t = intersection->second;
                  if (t >= 0.0f && t <= 1.0f) {
                     if (!closest.has_value() || t < std::get<1>(closest.value())) {
                        closest = std::make_optional(std::make_tuple(p, t, &other_segment));
                     }
                  }
               }
            }
         }
      }
   }

   return closest;
}

// Utility functions for intersection tests

std::optional<std::pair<glm::vec2, float>> intersect_ray_segment(const Ray& ray, const Segment& segment) {
   const glm::vec2& p = ray.origin;
   const glm::vec2& r = ray.direction;
   const glm::vec2& q = segment.start;
   glm::vec2        s = segment.end - segment.start;

   float r_cross_s = cross(r, s);
   if (std::abs(r_cross_s) < EPSILON) {
      // Lines are parallel
      return std::nullopt;
   }

   glm::vec2 q_minus_p = q - p;

   float t = cross(q_minus_p, s) / r_cross_s;
   float u = cross(q_minus_p, r) / r_cross_s;

   if (t >= 0.0f && u >= 0.0f && u <= 1.0f) {
      // Intersection occurs at p + t * r
      glm::vec2 intersection_point = p + t * r;
      return std::make_optional(std::make_pair(intersection_point, t));
   } else {
      return std::nullopt;
   }
}

std::optional<std::pair<glm::vec2, float>> intersect_segment_segment(const Segment& s1, const Segment& s2) {
   const glm::vec2& p = s1.start;
   glm::vec2        r = s1.end - s1.start;
   const glm::vec2& q = s2.start;
   glm::vec2        s = s2.end - s2.start;

   float r_cross_s = cross(r, s);
   if (std::abs(r_cross_s) < EPSILON) {
      // Lines are parallel
      return std::nullopt;
   }

   glm::vec2 q_minus_p = q - p;

   float t = cross(q_minus_p, s) / r_cross_s;
   float u = cross(q_minus_p, r) / r_cross_s;

   if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
      // Intersection occurs at p + t * r
      glm::vec2 intersection_point = p + t * r;
      return std::make_optional(std::make_pair(intersection_point, t));
   } else {
      return std::nullopt;
   }
}

std::optional<std::pair<float, float>> intersect_ray_aabb(const Ray& ray, const AABB& aabb) {
   float tmin = (aabb.min.x - ray.origin.x) / ray.direction.x;
   float tmax = (aabb.max.x - ray.origin.x) / ray.direction.x;

   if (tmin > tmax)
      std::swap(tmin, tmax);

   float tymin = (aabb.min.y - ray.origin.y) / ray.direction.y;
   float tymax = (aabb.max.y - ray.origin.y) / ray.direction.y;

   if (tymin > tymax)
      std::swap(tymin, tymax);

   if ((tmin > tymax) || (tymin > tmax))
      return std::nullopt;

   if (tymin > tmin)
      tmin = tymin;

   if (tymax < tmax)
      tmax = tymax;

   if (tmax < 0.0f)
      // Intersection is behind the ray origin
      return std::nullopt;

   return std::make_optional(std::make_pair(tmin, tmax));
}

bool intersect_segment_aabb(const Segment& segment, const AABB& aabb) {
   float t0 = 0.0f;
   float t1 = 1.0f;
   float dx = segment.end.x - segment.start.x;
   float dy = segment.end.y - segment.start.y;

   float p[4] = {-dx, dx, -dy, dy};
   float q[4] = {segment.start.x - aabb.min.x, aabb.max.x - segment.start.x, segment.start.y - aabb.min.y,
                 aabb.max.y - segment.start.y};

   for (int i = 0; i < 4; ++i) {
      if (std::abs(p[i]) < EPSILON) {
         if (q[i] < 0.0f)
            return false;
      } else {
         float r = q[i] / p[i];
         if (p[i] < 0.0f) {
            if (r > t1)
               return false;
            else if (r > t0)
               t0 = r;
         } else {
            if (r < t0)
               return false;
            else if (r < t1)
               t1 = r;
         }
      }
   }
   return true;
}
