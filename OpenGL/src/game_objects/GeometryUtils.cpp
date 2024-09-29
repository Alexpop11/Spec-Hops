// PolygonUtils.cpp

#include "GeometryUtils.h"
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>

#include "../Renderer.h"

namespace GeometryUtils {

float length2(const glm::vec2& a, const glm::vec2& b) {
   glm::vec2 temp = a - b;
   return glm::dot(temp, temp);
}

bool findPolygonUnion(const std::vector<std::vector<glm::vec2>>& polygons, PolyTreeD& output) {
   ClipperD clipper;
   for (const auto& polygon : polygons) {
      if (!polygon.empty()) {
         PathsD subject;
         PathD  path;
         path.reserve(polygon.size());
         for (const auto& point : polygon) {
            path.emplace_back(static_cast<double>(point.x), static_cast<double>(point.y));
         }
         subject.push_back(path);
         clipper.AddSubject(subject);
      }
   }
   return clipper.Execute(ClipType::Union, FillRule::Positive, output);
}

PathsD FlattenPolyPathD(const PolyPathD& polyPath) {
   PathsD paths;

   // Lambda function for recursive traversal
   std::function<void(const PolyPathD&)> traverse = [&](const PolyPathD& node) {
      // Add the current node's polygon to the paths
      paths.emplace_back(node.Polygon());

      // Recursively traverse each child
      for (auto it = node.begin(); it != node.end(); ++it) {
         if (*it) { // Check if the child exists
            traverse(*(*it));
         }
      }
   };

   // Start traversal from the root node
   traverse(polyPath);

   paths = SimplifyPaths(paths, 0.025);

   return paths;
}


// Helper function to compute intersection between a ray and a segment
std::optional<glm::vec2> RaySegmentIntersect(const glm::vec2& ray_origin, double dx, double dy, const glm::vec2& a,
                                             const glm::vec2& b) {
   double r_px = ray_origin.x;
   double r_py = ray_origin.y;
   double r_dx = dx;
   double r_dy = dy;

   double s_px = a.x;
   double s_py = a.y;
   double s_dx = b.x - a.x;
   double s_dy = b.y - a.y;

   double denominator = r_dx * s_dy - r_dy * s_dx;

   if (std::fabs(denominator) < 1e-10) {
      // Lines are parallel
      return std::nullopt;
   }

   double t = ((s_px - r_px) * s_dy - (s_py - r_py) * s_dx) / denominator;
   double u = ((s_px - r_px) * r_dy - (s_py - r_py) * r_dx) / denominator;

   if (t >= 0 && u >= 0 && u <= 1) {
      // Intersection point
      glm::vec2 intersection_point;
      intersection_point.x = r_px + t * r_dx;
      intersection_point.y = r_py + t * r_dy;
      return intersection_point;
   } else {
      return std::nullopt;
   }
}

// Function to compute intersection between two line segments
std::optional<glm::vec2> LineSegmentIntersect(const glm::vec2& line1_start, const glm::vec2& line1_end,
                                              const glm::vec2& line2_start, const glm::vec2& line2_end) {
   double x1 = line1_start.x;
   double y1 = line1_start.y;
   double x2 = line1_end.x;
   double y2 = line1_end.y;

   double x3 = line2_start.x;
   double y3 = line2_start.y;
   double x4 = line2_end.x;
   double y4 = line2_end.y;

   double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

   if (std::fabs(denominator) < 1e-10) {
      // Lines are parallel
      return std::nullopt;
   }

   double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
   double u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / denominator;

   if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
      // Intersection point
      glm::vec2 intersection_point;
      intersection_point.x = x1 + t * (x2 - x1);
      intersection_point.y = y1 + t * (y2 - y1);
      return intersection_point;
   } else {
      return std::nullopt;
   }
}

// Struct to hold a point along with its angle and type
struct TaggedPoint {
   PointD point;
   double angle;
   double adjusted_angle;
   enum class PointType { START, MIDDLE, END } type;
};

std::vector<TaggedPoint> TagPoints(const glm::vec2& position, const PathD& polygon) {
   std::vector<TaggedPoint> tagged_points;

   // Compute angle for each vertex and collect them
   for (const auto& pt : polygon) {
      glm::vec2 vertex(pt.x, pt.y);
      double    dx    = vertex.x - position.x;
      double    dy    = vertex.y - position.y;
      double    angle = atan2(dy, dx);

      tagged_points.emplace_back(pt, angle, 0.0, TaggedPoint::PointType::MIDDLE);
   }

   if (tagged_points.empty()) {
      // Handle the case where polygon has no points
      return tagged_points;
   }

   if (PointInPolygon({position.x, position.y}, polygon) == PointInPolygonResult::IsInside) {
      // all points are inside, so all are middle
      for (auto& tp : tagged_points) {
         tp.type = TaggedPoint::PointType::MIDDLE;
      }
      return tagged_points;
   }

   // Find a reference angle (e.g., the angle of the first point)
   double ref_angle = tagged_points.front().angle;

   // Adjust angles to handle wrapping
   for (auto& tp : tagged_points) {
      double angle_diff = tp.angle - ref_angle;

      // Normalize angle_diff to be within [-π, π)
      while (angle_diff < -M_PI)
         angle_diff += 2 * M_PI;
      while (angle_diff >= M_PI)
         angle_diff -= 2 * M_PI;

      // Store the adjusted angle relative to the reference angle
      tp.adjusted_angle = angle_diff;
   }

   // Sort the vertices of the polygon by adjusted angle
   std::sort(tagged_points.begin(), tagged_points.end(),
             [](const TaggedPoint& a, const TaggedPoint& b) { return a.adjusted_angle < b.adjusted_angle; });

   // Calculate the total angle span
   double total_angle_span = tagged_points.back().adjusted_angle - tagged_points.front().adjusted_angle;

   // Tag the first as START, last as END, others as MIDDLE
   tagged_points.front().type = TaggedPoint::PointType::START;
   tagged_points.back().type  = TaggedPoint::PointType::END;


   return tagged_points;
}

// ComputeVisibilityPolygon function implementing the new algorithm
PathD ComputeVisibilityPolygon(const glm::vec2& position, const PathsD& obstacles) {


   std::vector<TaggedPoint> all_points;

   // For each obstacle
   for (const auto& polygon : obstacles) {
      std::vector<TaggedPoint> tagged_points = TagPoints(position, polygon);

      // Add tagged points to the global list
      all_points.insert(all_points.end(), tagged_points.begin(), tagged_points.end());
   }

   for (auto& point : all_points) {
      auto color = point.type == TaggedPoint::PointType::START ? glm::vec4(1, 0, 0, 1)
                   : point.type == TaggedPoint::PointType::END ? glm::vec4(0, 1, 0, 1)
                                                               : glm::vec4(1, 1, 1, 1);
      Renderer::DebugLine({point.point.x, point.point.y}, {point.point.x + 0.1, point.point.y + 0.1}, color);
   }

   // Sort all points globally by angle
   std::sort(all_points.begin(), all_points.end(),
             [](const TaggedPoint& a, const TaggedPoint& b) { return a.angle < b.angle; });

   PathD visibility_polygon;

   // For each point in the sorted list
   for (const auto& tagged_point : all_points) {
      const PointD& point = tagged_point.point;
      glm::vec2     vertex(point.x, point.y);

      // Perform a linecast from position to the point
      bool obstructed = false;

      // Check if the line from position to point intersects any obstacle before the point
      for (const auto& polygon : obstacles) {
         size_t count = polygon.size();
         for (size_t i = 0; i < count; ++i) {
            const PointD& pa = polygon[i];
            const PointD& pb = polygon[(i + 1) % count];

            // Skip if the edge is adjacent to the point to avoid false positives
            if ((pa == point) || (pb == point))
               continue;

            glm::vec2 a(pa.x, pa.y);
            glm::vec2 b(pb.x, pb.y);

            // Check for intersection
            auto intersection_opt = LineSegmentIntersect(position, vertex, a, b);
            if (intersection_opt) {
               double dist_to_intersection = length2(intersection_opt.value(), position);
               double dist_to_point        = length2(vertex, position);

               // If the intersection is closer than the point, it's obstructed
               if (dist_to_intersection < dist_to_point) {
                  obstructed = true;
                  break;
               }
            }
         }
         if (obstructed)
            break;
      }

      if (!obstructed) {
         // Point is visible
         if (tagged_point.type == TaggedPoint::PointType::START) {
            // Raycast starting from the point in the same direction
            glm::vec2 direction = glm::normalize(vertex - position);
            glm::vec2 ray_end   = vertex + direction * 1e6f; // Large distance

            glm::vec2 closest_intersection = ray_end;
            double    min_distance         = std::numeric_limits<double>::max();

            // Find the closest intersection point beyond the point
            for (const auto& polygon : obstacles) {
               size_t count = polygon.size();
               for (size_t i = 0; i < count; ++i) {
                  const PointD& pa = polygon[i];
                  const PointD& pb = polygon[(i + 1) % count];

                  // Skip if the edge is adjacent to the point
                  if ((pa == point) || (pb == point))
                     continue;

                  glm::vec2 a(pa.x, pa.y);
                  glm::vec2 b(pb.x, pb.y);

                  auto intersection_opt = LineSegmentIntersect(vertex, ray_end, a, b);
                  if (intersection_opt) {
                     glm::vec2 intersection_point = intersection_opt.value();
                     double    distance           = length2(intersection_point, vertex);

                     if (distance < min_distance) {
                        min_distance         = distance;
                        closest_intersection = intersection_point;
                     }
                  }
               }
            }

            // Add the intersection point and the starting point to the visibility polygon
            visibility_polygon.emplace_back(closest_intersection.x, closest_intersection.y);
            visibility_polygon.emplace_back(point.x, point.y);
         } else if (tagged_point.type == TaggedPoint::PointType::MIDDLE) {
            // Add the middle point to the visibility polygon
            visibility_polygon.emplace_back(point.x, point.y);
         } else if (tagged_point.type == TaggedPoint::PointType::END) {
            // Add the ending point to the visibility polygon
            visibility_polygon.emplace_back(point.x, point.y);

            // Raycast starting from the point in the same direction
            glm::vec2 direction = glm::normalize(vertex - position);
            glm::vec2 ray_end   = vertex + direction * 1e6f; // Large distance

            glm::vec2 closest_intersection = ray_end;
            double    min_distance         = std::numeric_limits<double>::max();

            // Find the closest intersection point beyond the point
            for (const auto& polygon : obstacles) {
               size_t count = polygon.size();
               for (size_t i = 0; i < count; ++i) {
                  const PointD& pa = polygon[i];
                  const PointD& pb = polygon[(i + 1) % count];

                  // Skip if the edge is adjacent to the point
                  if ((pa == point) || (pb == point))
                     continue;

                  glm::vec2 a(pa.x, pa.y);
                  glm::vec2 b(pb.x, pb.y);

                  auto intersection_opt = LineSegmentIntersect(vertex, ray_end, a, b);
                  if (intersection_opt) {
                     glm::vec2 intersection_point = intersection_opt.value();
                     double    distance           = length2(intersection_point, vertex);

                     if (distance < min_distance) {
                        min_distance         = distance;
                        closest_intersection = intersection_point;
                     }
                  }
               }
            }

            // Add the intersection point to the visibility polygon
            visibility_polygon.emplace_back(closest_intersection.x, closest_intersection.y);
         }
      }
   }

   // The visibility polygon is already sorted by angle
   return visibility_polygon;
}


} // namespace GeometryUtils
