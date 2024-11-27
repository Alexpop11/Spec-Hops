#include "SceneGeometry.h"
#include "GeometryUtils.h"

using namespace Clipper2Lib;
using namespace GeometryUtils;

SceneGeometry::GeometryResult SceneGeometry::computeSceneGeometry(const std::vector<std::vector<glm::vec2>>& allBounds,
                                                                  const glm::vec2& playerPosition, bool showWalls) {
   GeometryResult result =
      GeometryResult{std::make_unique<Clipper2Lib::PolyTreeD>(), std::make_unique<Clipper2Lib::PolyTreeD>()};

   // Compute the union of all tile bounds
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   auto flattened = FlattenPolyPathD(combined);

   // Prepare the hull for clipping
   ClipperD clipper;
   PathsD   hullPaths;
   for (auto& child : combined) {
      hullPaths.push_back(child->Polygon());
   }
   clipper.AddSubject(hullPaths);

   // Compute the visibility polygon
   auto visibility = ComputeVisibilityPolygon(playerPosition, flattened);

   // Compute the areas occluded
   clipper.AddClip({visibility});
   if (showWalls) {
      clipper.AddClip({flattened});
   }
   // Compute the difference to get invisibility regions
   clipper.Execute(ClipType::Difference, FillRule::NonZero, *result.invisibilityPaths);

   if (showWalls) {
      // Tint all the walls that are not visible
      ClipperD tint;
      tint.AddSubject({flattened});
      tint.AddClip({visibility});
      tint.Execute(ClipType::Difference, FillRule::NonZero, *result.wallPaths);
   }

   return result;
}

Clipper2Lib::PathsD SceneGeometry::getFlattenedBounds(const std::vector<std::vector<glm::vec2>>& allBounds) {
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   return FlattenPolyPathD(combined);
}
