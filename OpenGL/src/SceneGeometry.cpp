#include "SceneGeometry.h"
#include "GeometryUtils.h"

using namespace Clipper2Lib;
using namespace GeometryUtils;

SceneGeometry::WallResult SceneGeometry::computeWallPaths(const std::vector<std::vector<glm::vec2>>& allBounds,
                                                          const glm::vec2&                           playerPosition) {

   WallResult result;
   result.wallPaths = std::make_unique<PolyTreeD>();

   // Compute the union of all tile bounds
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   result.flattened = FlattenPolyPathD(combined);

   return result;
}


SceneGeometry::VisibilityResult
SceneGeometry::computeSceneGeometry(const std::vector<std::vector<glm::vec2>>& allBounds,
                                    const glm::vec2& playerPosition, bool showWalls) {

   SceneGeometry::VisibilityResult result{std::make_unique<PolyTreeD>(), std::make_unique<PolyTreeD>()};

   // Compute the visibility polygon
   result.visibility = ComputeVisibilityPolygon(playerPosition, result.flattened);

   // First compute wall paths and visibility
   auto wallResult = computeWallPaths(allBounds, playerPosition);

   // Prepare the hull for clipping
   PathsD    hullPaths;
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   for (auto& child : combined) {
      hullPaths.push_back(child->Polygon());
   }

   // Compute invisibility paths
   result.invisibilityPaths = std::make_unique<PolyTreeD>();
   ClipperD clipper;
   clipper.AddSubject(hullPaths);
   clipper.AddClip({wallResult.visibility});
   if (showWalls) {
      clipper.AddClip({wallResult.flattened});
   }
   clipper.Execute(ClipType::Difference, FillRule::NonZero, *result.invisibilityPaths);

   if (showWalls) {
      result.wallPaths = std::move(wallResult.wallPaths);
   }

   return result;
}
