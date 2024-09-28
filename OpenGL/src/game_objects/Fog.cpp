#include "Fog.h"
#include <array>
#include "Tile.h"
#include <ranges>
#include <vector>

Fog::Fog()
   : GameObject("Fog of War", 6, {0, 0}) {
   
}

void Fog::setUpShader(Renderer& renderer) {

}

void Fog::render(Renderer& renderer) {
   std::vector<std::vector<glm::vec2>> allBounds;
   auto Tiles = World::where<Tile>(
         [&](const Tile& tile) { return true; });
   for (auto Tile : Tiles) {
      auto Bounds = Tile->getBounds();
      for (size_t i = 0; i + 1 < Bounds.size(); ++i) { // Manual sliding window
         renderer.Line(Bounds[i], Bounds[i + 1], {1, 0, 0});
      }
   }
}

void Fog::update() {
   // Update logic for Background
}
