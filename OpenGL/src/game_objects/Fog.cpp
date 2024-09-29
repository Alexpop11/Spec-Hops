#include "Fog.h"
#include <array>
#include "Tile.h"
#include <ranges>
#include <vector>
#include "clipper2/clipper.h"
using namespace Clipper2Lib;

Fog::Fog()
   : GameObject("Fog of War", 6, {0, 0}) {}

void Fog::setUpShader(Renderer& renderer) {}

PathsD findPolygonUnion(const std::vector<std::vector<glm::vec2>>& polygons) {
   PathsD subject;
   for (const auto& polygon : polygons) {
      PathD path = MakePathD<double>({});
      for (const auto& point : polygon) {
         path.emplace_back(point.x, point.y);
      }
      subject.push_back(path);
   }
   auto solution = Union(subject, FillRule::NonZero);
   return solution;
}


void Fog::render(Renderer& renderer) {
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::where<Tile>([&](const Tile& tile) { return true; });
   for (auto tile : tiles) {
      auto bounds = tile->getBounds();
      allBounds.push_back(bounds);
   }
   auto combined = findPolygonUnion(allBounds);
   for (const auto& path : combined) {
      for (size_t i = 0; i < path.size(); ++i) {
         auto start = path[i];
         auto end   = path[(i + 1) % path.size()];
         renderer.Line({start.x, start.y}, {end.x, end.y}, {1, 0, 0});
      }
   }
}

void Fog::update() {
   // Update logic for Background
}
