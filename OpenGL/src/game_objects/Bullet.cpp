#include "Bullet.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Bullet::Bullet(const std::string& name, float x, float y, int direction_x, int direction_y)
   : SquareObject(name, DrawPriority::Bomb, x, y, "bullet.png")
   , direction_x(direction_x)
   , direction_y(direction_y) {}

void Bullet::tickUpdate() {

   // Check if the bullet hits a wall
   auto tiles = World::at<Tile>(tile_x, tile_y);
   for (auto tile : tiles) {
      if (tile->wall) {
         ShouldDestroy = true;
         return; // Stop further processing since the bullet is destroyed
      }
   }

   // Assuming you have a way to check for characters at the new position
   auto nearbyCharacters = World::at<Character>(tile_x, tile_y);
   if (!nearbyCharacters.empty()) {
      auto character = nearbyCharacters.front(); // Assuming we target the first found character
      character->hurt();
      std::cout << "Bullet damaged character. Their health is now " << character->health << std::endl;
      ShouldDestroy = true;
   }

   // Move the bullet
   tile_x += direction_x;
   tile_y += direction_y;
}
