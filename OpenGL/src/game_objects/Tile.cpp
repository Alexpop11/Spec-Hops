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
      r = zeno(r, 0.4f, 0.1);
      g = zeno(g, 0.4f, 0.1);
      b = zeno(b, 0.4f, 0.1);

   } else if (wall == true) {
      r = 0.3f;
      g = 0.3f;
      b = 0.3f;
   } else {
      r = zeno(r, 0.5f, 0.1);
      g = zeno(g, 0.5f, 0.1);
      b = zeno(b, 0.5f, 0.1);
   }
}
