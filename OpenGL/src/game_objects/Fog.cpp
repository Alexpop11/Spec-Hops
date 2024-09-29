#include "Fog.h"
#include <array>
#include "Tile.h"
#include <ranges>
#include <vector>
#include "clipper2/clipper.h"
#include "GeometryUtils.h"
using namespace Clipper2Lib;
using namespace GeometryUtils;

Fog::Fog()
   : GameObject("Fog of War", 6, {0, 0}) {}

void Fog::setUpShader(Renderer& renderer) {}

void Fog::render(Renderer& renderer) {
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::where<Tile>([&](const Tile& tile) { return true; });
   for (auto tile : tiles) {
      auto bounds = tile->getBounds();
      allBounds.push_back(bounds);
   }

   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);

   for (const auto& path : combined) {
      auto polygon = path->Polygon();

      auto flattened = FlattenPolyPathD(*path);
      for (const auto& polygon : flattened) {
         for (size_t i = 0; i < polygon.size(); ++i) {
            auto start = polygon[i];
            auto end   = polygon[(i + 1) % polygon.size()];
            renderer.Line({start.x, start.y}, {end.x, end.y}, {0, 1, 0});
         }
      }

      for (size_t i = 0; i < polygon.size(); ++i) {
         auto start = polygon[i];
         auto end   = polygon[(i + 1) % polygon.size()];
         renderer.Line({start.x, start.y}, {end.x, end.y}, {1, 0, 0});
      }
   }
}

void Fog::update() {
   // Update logic for Background
}
