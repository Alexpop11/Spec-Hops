#include "SceneGeometry.h"
#include "GeometryUtils.h"

using namespace Clipper2Lib;
using namespace GeometryUtils;

SceneGeometry::WallResult SceneGeometry::computeWallPaths(
   const std::vector<std::vector<glm::vec2>>& allBounds,
   const glm::vec2& playerPosition) {
   
   WallResult result;
   result.wallPaths = std::make_unique<PolyTreeD>();

   // Compute the union of all tile bounds
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   result.flattened = FlattenPolyPathD(combined);

   // Compute the visibility polygon
   result.visibility = ComputeVisibilityPolygon(playerPosition, result.flattened);

   // Tint all the walls that are not visible
   ClipperD tint;
   tint.AddSubject({result.flattened});
   tint.AddClip({result.visibility});
   tint.Execute(ClipType::Difference, FillRule::NonZero, *result.wallPaths);

   return result;
}

std::unique_ptr<Clipper2Lib::PolyTreeD> SceneGeometry::computeInvisibilityPaths(
   const PathsD& hullPaths,
   const PathD& visibility,
   const PathsD& flattened,
   bool showWalls) {
   
   auto result = std::make_unique<PolyTreeD>();
   
   ClipperD clipper;
   clipper.AddSubject(hullPaths);
   clipper.AddClip({visibility});
   if (showWalls) {
      clipper.AddClip({flattened});
   }
   
   clipper.Execute(ClipType::Difference, FillRule::NonZero, *result);
   return result;
}

SceneGeometry::GeometryResult SceneGeometry::computeSceneGeometry(
   const std::vector<std::vector<glm::vec2>>& allBounds,
   const glm::vec2& playerPosition,
   bool showWalls) {
   
   GeometryResult result{
      std::make_unique<PolyTreeD>(),
      std::make_unique<PolyTreeD>()
   };

   // First compute wall paths and visibility
   auto wallResult = computeWallPaths(allBounds, playerPosition);
   
   // Prepare the hull for clipping
   PathsD hullPaths;
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   for (auto& child : combined) {
      hullPaths.push_back(child->Polygon());
   }

   // Compute invisibility paths
   result.invisibilityPaths = computeInvisibilityPaths(
      hullPaths,
      wallResult.visibility,
      wallResult.flattened,
      showWalls
   );

   if (showWalls) {
      result.wallPaths = std::move(wallResult.wallPaths);
   }

   return result;
}

Clipper2Lib::PathsD SceneGeometry::getFlattenedBounds(const std::vector<std::vector<glm::vec2>>& allBounds) {
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   return FlattenPolyPathD(combined);
}
