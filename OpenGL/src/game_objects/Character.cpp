#include "Character.h"

Character::Character(const std::string& name, int x, int y)
   : SquareObject(name, 2, x, y) {}

void Character::update() {
   x = x + 0.6 * (tile_x - x);
   y = y + 0.6 * (tile_y - y);
}

void Character::tickUpdate() {}

void Character::move(int new_x, int new_y) {
   bool new_spot_occupied = false;
   for (auto& tile : World::at<Tile>(new_x, new_y)) {
      if (tile->wall) {
         new_spot_occupied = true;
      }
   };
   for (auto& tile : World::at<Character>(new_x, new_y)) {
      new_spot_occupied = true;
   };

   if (!new_spot_occupied) {
      tile_x = new_x;
      tile_y = new_y;
   }
}

void Character::die() {
   r = 0.4f;
   g = 0.3f;
   b = 0.3f;
}
