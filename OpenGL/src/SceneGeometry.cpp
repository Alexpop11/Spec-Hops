#include "SceneGeometry.h"
#include "GeometryUtils.h"
#include "World.h"
#include "game_objects/Tile.h"

using namespace Clipper2Lib;
using namespace GeometryUtils;

SceneGeometry::WallResult SceneGeometry::computeWallPaths() {
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::getAll<Tile>(); // Simplified retrieval of all tiles
   for (auto tile : tiles) {
      allBounds.push_back(tile->getBounds());
   }

   WallResult result;
   result.allBounds = allBounds;
   result.wallPaths = std::make_unique<PolyTreeD>();

   // Compute the union of all tile bounds
   findPolygonUnion(allBounds, *result.wallPaths);
   result.flattened = FlattenPolyPathD(*result.wallPaths);

   return result;
}


SceneGeometry::VisibilityResult SceneGeometry::computeVisibility(SceneGeometry::WallResult& wallResult,
                                                                 const glm::vec2&           playerPosition) {
   SceneGeometry::VisibilityResult result{Clipper2Lib::PathD(), std::make_unique<PolyTreeD>()};

   // Compute the visibility polygon
   result.visibility = ComputeVisibilityPolygon(playerPosition, wallResult.flattened);

   // Prepare the hull for clipping
   PathsD    hullPaths;
   PolyTreeD combined;
   findPolygonUnion(wallResult.allBounds, combined);
   for (auto& child : combined) {
      hullPaths.push_back(child->Polygon());
   }

   // Compute invisibility paths
   result.invisibilityPaths = std::make_unique<PolyTreeD>();
   ClipperD clipper;
   clipper.AddSubject(hullPaths);
   clipper.AddClip({result.visibility});
   clipper.AddClip({wallResult.flattened});
   clipper.Execute(ClipType::Difference, FillRule::NonZero, *result.invisibilityPaths);

   return result;
}
