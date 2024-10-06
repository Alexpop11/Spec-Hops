#include "Bomb.h"
#include "../World.h"
#include "Tile.h"
#include "Player.h"

Bomb::Bomb(const std::string& name, float x, float y)
   : Entity(name, DrawPriority::Bomb, x, y, "textures/bomb.png") {
   ExplodeTick = 0;
}

void Bomb::tickUpdate() {
   tintColor.a = zeno(tintColor.a, 0.0, 0.5);
   // Explode the bomb
   if (ExplodeTick > 6) {
      explode();
   } else {
      ExplodeTick++;
   }
}

void Bomb::explode() {
   auto nearbyWalls = World::where<Tile>(
      [&](const Tile& tile) { return (std::abs(tile_x - tile.tile_x) + std::abs(tile_y - tile.tile_y) < 3); });

   for (auto wall : nearbyWalls) {
      wall->explode();
   }

   auto nearbyCharacters = World::where<Character>([&](const Character& character) {
      return (std::abs(tile_x - character.tile_x) + std::abs(tile_y - character.tile_y) < 3);
   });
   for (auto character : nearbyCharacters) {
      character->hurt();
      std::cout << "bomb damaged " << character->name << ". their health is now " << character->health << std::endl;
   }

   audio().Bomb_Sound.play();
   ShouldDestroy = true;
}

void Bomb::kick(bool hitWall) {
   if (hitWall) {
      explode();
   }
}
