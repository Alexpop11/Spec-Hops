#include "Player.h"
#include "Bomb.h"
#include "../World.h"
#include "Tile.h"
#include "AudioEngine.h"
#include "Decal.h"

Bomb::Bomb(const std::string& name, float x, float y)
   : Entity(name, DrawPriority::Bomb, x, y, "bomb.png") {
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
   auto nearbyWalls = World::where<Tile>([&](const Tile& tile) {
      return (std::abs(getTile().x - tile.getTile().x) + std::abs(getTile().y - tile.getTile().y) < 3);
   });

   for (auto wall : nearbyWalls) {
      wall->explode();
   }

   auto nearbyCharacters = World::where<Character>([&](const Character& character) {
      return (std::abs(getTile().x - character.getTile().x) + std::abs(getTile().y - character.getTile().y) < 3);
   });
   for (auto character : nearbyCharacters) {
      character->hurt();
      std::cout << "bomb damaged " << character->name << ". their health is now " << character->health << std::endl;
   }

   audio().Bomb_Sound.play();
   World::gameobjectstoadd.push_back(std::make_unique<Decal>("ExplosionDecal", getTile().x, getTile().y, "crater"));
   World::gameobjectstoadd.push_back(std::make_unique<Decal>("ExplosionDecal", getTile().x, getTile().y, "explosion"));
   ShouldDestroy = true;
}

void Bomb::kick(bool hitWall, int dx, int dy) {
   Entity::kick(hitWall, dx, dy);

   if (hitWall) {
      explode();
   }
}
