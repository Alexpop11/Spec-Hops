#include "Mine.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Mine::Mine(const std::string& name, float x, float y)
   : Bomb(name, x, y) {
   r           = 0.2f;
   g           = 0.2f;
   b           = 0.2f;
   ExplodeTick = 0;
}

void Mine::tickUpdate() {
   // Explode the Mine

   auto nearbyCharacters = World::where<Character>([&](const Character& character) {
      return (std::abs(x - character.tile_x) + std::abs(y - character.tile_y) < 3);
   });
   if (!nearbyCharacters.empty() || detectedCharacter) {
      detectedCharacter = true;
      ExplodeTick++;
      if (!red_last_frame) {
         // reset red_last_frame and make mine red
         red_last_frame = true;
         r              = 0.8;
         g              = 0.2;
         b              = 0.2;
      } else {
         r              = 0.2f;
         g              = 0.2f;
         b              = 0.2f;
         red_last_frame = false;
      }
      if (ExplodeTick > 24) {

         auto nearbyWalls = World::where<Tile>(
            [&](const Tile& tile) { return (std::abs(x - tile.tile_x) + std::abs(y - tile.tile_y) < 3); });

         for (auto* wall : nearbyWalls) {
            wall->explode();
         }
         auto nearbyCharacters = World::where<Character>([&](const Character& character) {
            return (std::abs(x - character.tile_x) + std::abs(y - character.tile_y) < 3);
         });
         for (auto* character : nearbyCharacters) {
            character->health -= 1;
            std::cout << "Mine damaged player. their health is now " << character->health << std::endl;
         }
         ShouldDestroy = true;
      }
   }
}
