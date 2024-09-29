// PolygonUtils.cpp

#include "GeometryUtils.h"
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>
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

// Function to compute the visibility polygon from the player's position
PathD ComputeVisibilityPolygon(const glm::vec2& position, const PathsD& obstacles) {
   // Collect all the unique angles to vertices, and slightly offset angles
   std::vector<double> angles;

   const double EPSILON = 1e-9;

   // For each obstacle
   for (const auto& polygon : obstacles) {
      // For each vertex in the polygon
      for (const auto& point : polygon) {
         glm::vec2 vertex(point.x, point.y);
         double    dx         = vertex.x - position.x;
         double    dy         = vertex.y - position.y;
         double    base_angle = atan2(dy, dx);

         // Add base angle and slight offsets
         angles.push_back(base_angle - EPSILON);
         angles.push_back(base_angle);
         angles.push_back(base_angle + EPSILON);
      }
   }

   // Remove duplicate angles
   std::sort(angles.begin(), angles.end());
   angles.erase(std::unique(angles.begin(), angles.end()), angles.end());

   PathD visibility_polygon;

   // For each angle
   for (const auto& angle : angles) {
      double dx = cos(angle);
      double dy = sin(angle);

      // Define the ray endpoint far away
      glm::vec2 ray_end = position + glm::vec2(dx, dy) * 1e6f; // Large distance

      // Find the closest intersection point with the obstacles
      glm::vec2 closest_intersection;
      double    min_distance = std::numeric_limits<double>::max();

      // For each obstacle
      for (const auto& polygon : obstacles) {
         // For each edge in the polygon
         size_t count = polygon.size();
         for (size_t i = 0; i < count; ++i) {
            const PointD& pa = polygon[i];
            const PointD& pb = polygon[(i + 1) % count];

            glm::vec2 a(pa.x, pa.y);
            glm::vec2 b(pb.x, pb.y);

            // Compute intersection between ray and segment [a,b]
            auto intersection_opt = LineSegmentIntersect(position, ray_end, a, b);
            if (intersection_opt) {
               glm::vec2 intersection_point = intersection_opt.value();
               // Compute squared distance from position to intersection point
               double distance = length2(intersection_point, position);
               if (distance < min_distance) {
                  min_distance         = distance;
                  closest_intersection = intersection_point;
               }
            }
         }
      }

      // If we found an intersection, add it to the visibility polygon
      if (min_distance < std::numeric_limits<double>::max()) {
         visibility_polygon.emplace_back(closest_intersection.x, closest_intersection.y);
      }
   }

   // Sort the visibility polygon points by angle
   std::sort(visibility_polygon.begin(), visibility_polygon.end(), [&position](const PointD& a, const PointD& b) {
      double angle_a = atan2(a.y - position.y, a.x - position.x);
      double angle_b = atan2(b.y - position.y, b.x - position.x);
      return angle_a < angle_b;
   });

   return visibility_polygon;
}


} // namespace GeometryUtils
