#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "clipper2/clipper.h"

class SceneGeometry {
public:
   struct WallResult {
      Clipper2Lib::PathsD                     flattened;
      std::unique_ptr<Clipper2Lib::PolyTreeD> wallPaths;
   };

   struct VisibilityResult {
      Clipper2Lib::PathD                      visibility;
      std::unique_ptr<Clipper2Lib::PolyTreeD> invisibilityPaths;
      std::unique_ptr<Clipper2Lib::PolyTreeD> wallPaths;
   };

   static WallResult computeWallPaths(const std::vector<std::vector<glm::vec2>>& allBounds,
                                      const glm::vec2&                           playerPosition);

   static VisibilityResult computeSceneGeometry(const std::vector<std::vector<glm::vec2>>& allBounds,
                                                const glm::vec2& playerPosition, bool showWalls);

private:
};
