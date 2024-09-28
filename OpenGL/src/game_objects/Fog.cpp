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
   for (auto TileWindow : Tiles | std::ranges::slide_view(2)) {
      if (TileWindow.size() == 2) {
         renderer.Line(TileWindow[0], TileWindow[1], 0.1);
      }
   }
}

void Fog::update() {
   // Update logic for Background
}
