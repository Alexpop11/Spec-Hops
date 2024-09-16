#include "Bomb.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Bomb::Bomb(const std::string& name, float x, float y)
   : SquareObject(name, 4, x, y) {
   r           = 0.2f;
   g           = 0.2f;
   b           = 0.2f;
   ExplodeTick = 0;
}

void Bomb::tickUpdate() {
   // Explode the bomb
   if (ExplodeTick > 24) {
      auto nearbyWalls =
         World::where<Tile>([&](const Tile& tile) { return (std::abs(x - tile.x) + std::abs(y - tile.y) < 3); });

      for (auto* wall : nearbyWalls) {
         wall->explode();
      }

      auto nearbyCharacters = World::where<Character>([&](const Character& character) {
         return (std::abs(x - character.tile_x) + std::abs(y - character.tile_y) < 3);
      });
      for (auto* character : nearbyCharacters) {
         character->health -= 1;
         std::cout << "bomb damaged player. their health is now " << character->health << std::endl;
      }

      audio().Bomb_Sound.play();
      ShouldDestroy = true;
   } else {
      ExplodeTick++;
   }
}
