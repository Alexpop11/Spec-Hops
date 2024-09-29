#include "Fog.h"
#include <array>
#include "Tile.h"
#include "Player.h"
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

   auto player = World::where<Player>([&](const Player& player) { return true; })[0];

   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   auto flattened = FlattenPolyPathD(combined);

   for (auto& polygon : flattened) {
      for (int i = 0; i < polygon.size(); i++) {
         auto& point     = polygon[i];
         auto& nextPoint = polygon[(i + 1) % polygon.size()];
         Renderer::DebugLine({point.x, point.y}, {nextPoint.x, nextPoint.y}, glm::vec3(1, 0, 1));
      }
   }

   auto visibilityPolygon = ComputeVisibilityPolygon(player->position, flattened);
   for (auto i = 0; i < visibilityPolygon.size(); i++) {
      auto& point     = visibilityPolygon[i];
      auto& nextPoint = visibilityPolygon[(i + 1) % visibilityPolygon.size()];
      Renderer::DebugLine({point.x, point.y}, {nextPoint.x, nextPoint.y}, glm::vec3(1, 1, 1));
   }
}

void Fog::update() {
   // Update logic for Background
}
