#include "Mine.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Mine::Mine(const std::string& name, float x, float y)
   : Bomb(name, x, y) {
   ExplodeTick = 0;
}

void Mine::tickUpdate() {
   // Explode the Mine

   auto nearbyCharacters = World::where<Character>([&](const Character& character) {
      return (std::abs(tile_x - character.tile_x) + std::abs(tile_y - character.tile_y) < 3);
   });
   if (!nearbyCharacters.empty() || detectedCharacter) {
      detectedCharacter = true;
      ExplodeTick++;
      if (!red_last_frame) {
         // reset red_last_frame and make mine red
         red_last_frame = true;
         tintColor.r    = 0.8;
         tintColor.g    = 0.2;
         tintColor.b    = 0.2;
         tintColor.a    = 1;
      } else {
         tintColor.a    = 0;
         audio().Bomb_Tick.play();
         red_last_frame = false;
      }
      if (ExplodeTick > 6) {

         auto nearbyWalls = World::where<Tile>(
            [&](const Tile& tile) { return (std::abs(tile_x - tile.tile_x) + std::abs(tile_y - tile.tile_y) < 3); });

         for (auto* wall : nearbyWalls) {
            wall->explode();
         }
         auto nearbyCharacters = World::where<Character>([&](const Character& character) {
            return (std::abs(tile_x - character.tile_x) + std::abs(tile_y - character.tile_y) < 3);
         });
         for (auto* character : nearbyCharacters) {
            character->hurt();
            std::cout << "Mine damaged player. their health is now " << character->health << std::endl;
         }
         audio().Bomb_Sound.play();
         ShouldDestroy = true;
      }
   }
}
