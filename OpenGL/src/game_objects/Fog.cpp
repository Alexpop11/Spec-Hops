#include "Fog.h"
#include <array>
#include "Tile.h"
#include <ranges>
#include <vector>

Fog::Fog()
   : GameObject("Fog of War", 6, {0, 0}) {}

void Fog::setUpShader(Renderer& renderer) {}

void Fog::render(Renderer& renderer) {
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::where<Tile>([&](const Tile& tile) { return true; });
   for (auto tile : tiles) {
      auto bounds = tile->getBounds();
      for (size_t i = 0; i + 1 < bounds.size(); ++i) { // Manual sliding window
         renderer.Line(bounds[i], bounds[i + 1], {1, 0, 0});
      }
   }
}

void Fog::update() {
   // Update logic for Background
}
