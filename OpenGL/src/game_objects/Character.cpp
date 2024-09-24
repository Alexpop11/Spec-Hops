#include "Character.h"
#include "Input.h"
#include <cmath>
#include <Renderer.h>


Character::Character(const std::string& name, int x, int y, std::string texturepath)
   : SquareObject(name, 2, x, y, texturepath) {}

void Character::update() {
   SquareObject::update();
}

void Character::tickUpdate() {
   // Powerup cooldowns
   if (bombCoolDown > 0) {
      bombCoolDown--;
   }
   if (bunnyHopCoolDown > 0) {
      bunnyHopCoolDown--;
   }
}

void Character::move(int new_x, int new_y) {
   int dx    = new_x - tile_x;
   int dy    = new_y - tile_y;
   int steps = std::max(std::abs(dx), std::abs(dy));

   for (int i = 1; i <= steps; ++i) {
      int check_x = tile_x + (dx * i) / steps;
      int check_y = tile_y + (dy * i) / steps;

      bool spot_occupied = false;
      for (auto& tile : World::at<Tile>(check_x, check_y)) {
         if (tile->wall) {
            spot_occupied = true;
            break;
         }
      }
      if (!spot_occupied) {
         for (auto& character : World::at<Character>(check_x, check_y)) {
            if (character != this) {
               spot_occupied = true;
               break;
            }
         }
      }

      if (spot_occupied) {
         // Move to the last unoccupied position
         if (i <= 1 && !hoppedLastTurn) {
            bunnyHopCoolDown = 0;
         }
         if (i > 1) {
            tile_x = tile_x + (dx * (i - 1)) / steps;
            tile_y = tile_y + (dy * (i - 1)) / steps;
         }
         return;
      }
   }

   // If we've made it here, the entire path is clear
   tile_x = new_x;
   tile_y = new_y;
}

void Character::die() {
   r = 0.4f;
   g = 0.3f;
   b = 0.3f;
}