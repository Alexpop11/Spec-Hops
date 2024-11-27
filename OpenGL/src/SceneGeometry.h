#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "clipper2/clipper.h"

class SceneGeometry {
public:
   struct WallResult {
      Clipper2Lib::PathsD flattened;
      Clipper2Lib::PathD visibility;
      std::unique_ptr<Clipper2Lib::PolyTreeD> wallPaths;
   };

   struct GeometryResult {
      std::unique_ptr<Clipper2Lib::PolyTreeD> invisibilityPaths;
      std::unique_ptr<Clipper2Lib::PolyTreeD> wallPaths;
   };

   static WallResult computeWallPaths(const std::vector<std::vector<glm::vec2>>& allBounds,
                                     const glm::vec2& playerPosition);

   static GeometryResult computeSceneGeometry(const std::vector<std::vector<glm::vec2>>& allBounds,
                                            const glm::vec2& playerPosition,
                                            bool showWalls);

private:
   static Clipper2Lib::PathsD getFlattenedBounds(const std::vector<std::vector<glm::vec2>>& allBounds);
   static std::unique_ptr<Clipper2Lib::PolyTreeD> computeInvisibilityPaths(
      const Clipper2Lib::PathsD& hullPaths,
      const Clipper2Lib::PathD& visibility,
      const Clipper2Lib::PathsD& flattened,
      bool showWalls);
};
