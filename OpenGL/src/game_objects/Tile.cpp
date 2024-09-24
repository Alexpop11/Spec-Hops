#include "Tile.h"


Tile::Tile(const std::string& name, bool wall, bool unbreakable, float x, float y)
   : SquareObject(name, 2, x, y, "Textures/wall.png")
   , wall(wall)
   , unbreakable(unbreakable) {}

Tile::Tile(const std::string& name, float x, float y)
   : Tile(name, false, true, x, y) {}

void Tile::explode() {
   if (!unbreakable || !wall) {
      tintColor = {0.8, 0.5, 0.5, 0.9};
      wall      = false;
   }
}

void Tile::update() {
   tintColor.a = zeno(tintColor.a, 0, 0.1);
}
