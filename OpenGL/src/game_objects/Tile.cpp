#include "Tile.h"


Tile::Tile(const std::string& name, bool wall, float x, float y)
   : SquareObject(name, 2, x, y)
   , wall(wall) {}

Tile::Tile(const std::string& name, float x, float y)
   : Tile(name, false, x, y) {}

void Tile::explode() {
   wall = false;

   r = 0.8f;
   g = 0.5f;
   b = 0.5f;
}

void Tile::update() {
   if (wall == true) {
      r = 0.4f;
      g = 0.4f;
      b = 0.4f;
   } else {
      r = 0.5f;
      g = 0.5f;
      b = 0.5f;
   }
}
