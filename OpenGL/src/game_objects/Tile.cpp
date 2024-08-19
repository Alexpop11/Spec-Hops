#include "Tile.h"


Tile::Tile(const std::string& name, bool wall, bool unbreakable, float x, float y)
   : SquareObject(name, 2, x, y)
   , wall(wall)
   , unbreakable(unbreakable) {}

Tile::Tile(const std::string& name, float x, float y)
   : Tile(name, false, true, x, y) {}

void Tile::explode() {
   r = 0.8f;
   g = 0.5f;
   b = 0.5f;
   if (!unbreakable) {
      wall = false;
   }
}

void Tile::update() {
   if (wall == true && !unbreakable) {
      r = 0.4f;
      g = 0.4f;
      b = 0.4f;

   } else if (wall == true) {
      r = 0.3f;
      g = 0.3f;
      b = 0.3f;
   } else {
      r = 0.5f;
      g = 0.5f;
      b = 0.5f;
   }
}
