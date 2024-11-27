#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "clipper2/clipper.h"

class SceneGeometry {
public:
   struct GeometryResult {
      std::unique_ptr<Clipper2Lib::PolyTreeD> invisibilityPaths;
      std::unique_ptr<Clipper2Lib::PolyTreeD> wallPaths;
   };

   static GeometryResult computeSceneGeometry(const std::vector<std::vector<glm::vec2>>& allBounds,
                                              const glm::vec2& playerPosition, bool showWalls);

private:
   static Clipper2Lib::PathsD getFlattenedBounds(const std::vector<std::vector<glm::vec2>>& allBounds);
};
