// PolygonUtils.cpp

#include "GeometryUtils.h"
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>
namespace GeometryUtils {

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
bool RaySegmentIntersect(const PointD& ray_origin, double dx, double dy, const PointD& a, const PointD& b,
                         PointD& intersection_point) {
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
      return false;
   }

   double t = ((s_px - r_px) * s_dy - (s_py - r_py) * s_dx) / denominator;
   double u = ((s_px - r_px) * r_dy - (s_py - r_py) * r_dx) / denominator;

   if (t >= 0 && u >= 0 && u <= 1) {
      // Intersection point
      intersection_point.x = r_px + t * r_dx;
      intersection_point.y = r_py + t * r_dy;
      return true;
   } else {
      return false;
   }
}

PathD ComputeVisibilityPolygon(const glm::vec2& pos, const PathsD& obstacles) {
   auto point = PointD{pos.x, pos.y};
   return ComputeVisibilityPolygon(point, obstacles);
}

// Function to compute the visibility polygon from the player's position
PathD ComputeVisibilityPolygon(const PointD& pos, const PathsD& obstacles) {
   // Collect all the unique angles to vertices, and slightly offset angles
   struct AngleInfo {
      double angle;
   };

   std::vector<double> angles;

   const double EPSILON = 1e-9;

   // For each obstacle
   for (const auto& polygon : obstacles) {
      // For each vertex in the polygon
      for (const auto& vertex : polygon) {
         double dx         = vertex.x - pos.x;
         double dy         = vertex.y - pos.y;
         double base_angle = atan2(dy, dx);

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

      // Find the closest intersection point with the obstacles
      PointD closest_intersection;
      double min_distance = std::numeric_limits<double>::max();

      // For each obstacle
      for (const auto& polygon : obstacles) {
         // For each edge in the polygon
         for (size_t i = 0; i < polygon.size(); ++i) {
            const PointD& a = polygon[i];
            const PointD& b = polygon[(i + 1) % polygon.size()];

            // Compute intersection between ray and segment [a,b]
            PointD intersection_point;
            if (RaySegmentIntersect(pos, dx, dy, a, b, intersection_point)) {
               // Compute squared distance from pos to intersection point
               double distance = (intersection_point.x - pos.x) * (intersection_point.x - pos.x) +
                                 (intersection_point.y - pos.y) * (intersection_point.y - pos.y);
               if (distance < min_distance) {
                  min_distance         = distance;
                  closest_intersection = intersection_point;
               }
            }
         }
      }

      // If we found an intersection, add it to the visibility polygon
      if (min_distance < std::numeric_limits<double>::max()) {
         visibility_polygon.push_back(closest_intersection);
      }
   }

   // Sort the visibility polygon points by angle
   std::sort(visibility_polygon.begin(), visibility_polygon.end(), [&pos](const PointD& a, const PointD& b) {
      double angle_a = atan2(a.y - pos.y, a.x - pos.x);
      double angle_b = atan2(b.y - pos.y, b.x - pos.x);
      return angle_a < angle_b;
   });

   return visibility_polygon;
}


} // namespace GeometryUtils
