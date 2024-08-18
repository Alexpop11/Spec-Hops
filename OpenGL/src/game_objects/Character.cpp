#include "Character.h"

Character::Character(const std::string& name, float x, float y)
   : SquareObject(name, 2, x, y) {}

void Character::update() {
   // Update logic for Character
}

void Character::move(float new_x, float new_y) {
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
      x = new_x;
      y = new_y;
   }
}

void Character::die() {
   r = 0.4f;
   g = 0.3f;
   b = 0.3f;
}
