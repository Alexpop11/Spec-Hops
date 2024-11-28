#include "BVH.h"
#include <algorithm>
#include <cmath>

const float EPSILON = 1e-8f;

bool BVHNode::is_leaf() const {
   return left == std::numeric_limits<size_t>::max() && right == std::numeric_limits<size_t>::max();
}

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

size_t BVH::build_recursive(size_t segment_start, size_t segment_end) {
   // Calculate AABB for current node
   AABB aabb = compute_aabb(segment_start, segment_end);

   // Create leaf node if we have few enough segments
   if (segment_end - segment_start <= 1) {
      BVHNode node;
      node.aabb          = aabb;
      node.left          = std::numeric_limits<size_t>::max();
      node.right         = std::numeric_limits<size_t>::max();
      node.segment_start = segment_start;
      node.segment_end   = segment_end;
      size_t node_idx    = nodes.size();
      nodes.push_back(node);
      return node_idx;
   }

   // Partition segments
   size_t mid = partition_segments(segment_start, segment_end);

   // Create internal node
   BVHNode node;
   node.aabb          = aabb;
   node.left          = 0; // Will be set later
   node.right         = 0;
   node.segment_start = segment_start;
   node.segment_end   = segment_end;
   size_t node_idx    = nodes.size();
   nodes.push_back(node);

   // Recursively build left and right children
   size_t left  = build_recursive(segment_start, mid);
   size_t right = build_recursive(mid, segment_end);

   // Update node with child indices
   nodes[node_idx].left  = left;
   nodes[node_idx].right = right;

   return node_idx;
}

AABB BVH::compute_aabb(size_t segment_start, size_t segment_end) const {
   float min_x = std::numeric_limits<float>::infinity();
   float min_y = std::numeric_limits<float>::infinity();
   float max_x = -std::numeric_limits<float>::infinity();
   float max_y = -std::numeric_limits<float>::infinity();

   for (size_t i = segment_start; i < segment_end; ++i) {
      const Segment& segment = segments[i];
      min_x                  = std::min(min_x, std::min(segment.start.x, segment.end.x));
      min_y                  = std::min(min_y, std::min(segment.start.y, segment.end.y));
      max_x                  = std::max(max_x, std::max(segment.start.x, segment.end.x));
      max_y                  = std::max(max_y, std::max(segment.start.y, segment.end.y));
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

   // Define a lambda to compute the centroid of a segment
   auto centroid = [axis_vertical](const Segment& segment) {
      if (axis_vertical) {
         return (segment.start.y + segment.end.y) / 2.0f;
      } else {
         return (segment.start.x + segment.end.x) / 2.0f;
      }
   };

   // Get a mutable iterator range for the segments
   auto segments_begin  = segments.begin() + segment_start;
   auto segments_end_it = segments.begin() + segment_end;

   // Sort the segments in place based on their centroids
   std::sort(segments_begin, segments_end_it,
             [centroid](const Segment& a, const Segment& b) { return centroid(a) < centroid(b); });

   // Return the midpoint index for splitting
   size_t mid = (segment_start + segment_end) / 2;
   return mid;
}

std::optional<std::pair<glm::vec2, const Segment*>> BVH::ray_intersect(const Ray& ray) const {
   if (nodes.empty()) {
      return std::nullopt;
   }
   auto result = ray_intersect_node(ray, 0, std::nullopt);
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
   const BVHNode& node = nodes[node_idx];

   // First, check if the ray intersects the node's AABB
   auto aabb_hit = intersect_ray_aabb(ray, node.aabb);
   if (aabb_hit.has_value()) {
      float tmin = aabb_hit->first;
      if (closest.has_value()) {
         float closest_t = std::get<1>(closest.value());
         if (tmin > closest_t) {
            // The node is further than the closest intersection found so far
            return closest;
         }
      }

      if (node.is_leaf()) {
         // Leaf node, check the segments
         auto new_closest = closest;
         for (size_t i = node.segment_start; i < node.segment_end; ++i) {
            const Segment& segment      = segments[i];
            auto           intersection = intersect_ray_segment(ray, segment);
            if (intersection.has_value()) {
               glm::vec2 p = intersection->first;
               float     t = intersection->second;
               if (t >= 0.0f) {
                  if (new_closest.has_value()) {
                     float closest_t = std::get<1>(new_closest.value());
                     if (t < closest_t) {
                        new_closest = std::make_optional(std::make_tuple(p, t, &segment));
                     }
                  } else {
                     new_closest = std::make_optional(std::make_tuple(p, t, &segment));
                  }
               }
            }
         }
         return new_closest;
      } else {
         // Internal node, traverse children
         auto left_result = ray_intersect_node(ray, node.left, closest);
         auto new_closest = left_result.has_value() ? left_result : closest;

         auto right_result = ray_intersect_node(ray, node.right, new_closest);
         return right_result.has_value() ? right_result : left_result;
      }
   } else {
      // Ray does not intersect this node's AABB
      return closest;
   }
}

std::optional<std::pair<glm::vec2, const Segment*>> BVH::segment_intersect(const Segment& segment) const {
   if (nodes.empty()) {
      return std::nullopt;
   }
   auto result = segment_intersect_node(segment, 0, std::nullopt);
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
   const BVHNode& node = nodes[node_idx];

   // First, check if the segment intersects the node's AABB
   if (intersect_segment_aabb(segment, node.aabb)) {
      if (node.is_leaf()) {
         // Leaf node, check the segments
         auto new_closest = closest;
         for (size_t i = node.segment_start; i < node.segment_end; ++i) {
            const Segment& other_segment = segments[i];
            auto           intersection  = intersect_segment_segment(segment, other_segment);
            if (intersection.has_value()) {
               glm::vec2 p = intersection->first;
               float     t = intersection->second;
               if (t >= 0.0f && t <= 1.0f) {
                  if (new_closest.has_value()) {
                     float closest_t = std::get<1>(new_closest.value());
                     if (t < closest_t) {
                        new_closest = std::make_optional(std::make_tuple(p, t, &other_segment));
                     }
                  } else {
                     new_closest = std::make_optional(std::make_tuple(p, t, &other_segment));
                  }
               }
            }
         }
         return new_closest;
      } else {
         // Internal node, traverse children
         auto left_result = segment_intersect_node(segment, node.left, closest);
         auto new_closest = left_result.has_value() ? left_result : closest;

         auto right_result = segment_intersect_node(segment, node.right, new_closest);
         return right_result.has_value() ? right_result : left_result;
      }
   } else {
      // Segment does not intersect this node's AABB
      return closest;
   }
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
