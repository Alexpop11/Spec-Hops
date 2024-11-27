#pragma once
#include <glm/glm.hpp>
#include "clipper2/clipper.h"

struct AABB {
   glm::vec2 min;
   glm::vec2 max;

   AABB()
      : min(std::numeric_limits<float>::max())
      , max(-std::numeric_limits<float>::max()) {}

   // Construct AABB from line segment endpoints
   AABB(const glm::vec2& p1, const glm::vec2& p2) {
      min.x = std::min(p1.x, p2.x);
      min.y = std::min(p1.y, p2.y);
      max.x = std::max(p1.x, p2.x);
      max.y = std::max(p1.y, p2.y);
   }

   bool intersects(const AABB& other) const {
      return !(other.min.x > max.x || other.max.x < min.x || other.min.y > max.y || other.max.y < min.y);
   }

   AABB merge(const AABB& other) const {
      return AABB(glm::vec2(std::min(min.x, other.min.x), std::min(min.y, other.min.y)),
                  glm::vec2(std::max(max.x, other.max.x), std::max(max.y, other.max.y)));
   }

   float surfaceArea() const {
      glm::vec2 diff = max - min;
      return diff.x * diff.y;
   }
};
