#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "clipper2/clipper.h"
#include "geometry/BVH.h"

class SceneGeometry {
public:
   struct WallResult {
      std::vector<std::vector<glm::vec2>>     allBounds;
      Clipper2Lib::PathsD                     flattened;
      std::unique_ptr<Clipper2Lib::PolyTreeD> wallPaths;
      std::unique_ptr<BVHNode>                bvh;
   };

   struct VisibilityResult {
      Clipper2Lib::PathD                      visibility;
      std::unique_ptr<Clipper2Lib::PolyTreeD> invisibilityPaths;
   };

   static WallResult computeWallPaths();

   static VisibilityResult computeVisibility(SceneGeometry::WallResult& wallResult, const glm::vec2& playerPosition);

private:
};
