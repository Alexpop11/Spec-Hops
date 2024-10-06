#include "Bomber.h"

Bomber::Bomber(const std::string& name, float x, float y)
   : Character(name, x, y, "textures/enemy.png") {
   drawPriority = DrawPriority::Character;
   health       = 1;
}

bool Bomber::move(int new_x, int new_y) {

   auto nearbyBombsCurrent = World::where<Bomb>(
      [&](const Bomb& bomb) { return (std::abs(tile_x - bomb.tile_x) + std::abs(tile_y - bomb.tile_y) < 3); });
   auto nearbyBombsNew = World::where<Bomb>(
      [&](const Bomb& bomb) { return (std::abs(new_x - bomb.tile_x) + std::abs(new_y - bomb.tile_y) < 3); });
   if (nearbyBombsNew.empty() || !nearbyBombsCurrent.empty()) {
      Character::move(new_x, new_y);
   }
   if (nearbyBombsCurrent.empty()) {
      for (auto& tile : World::at<Tile>(new_x, new_y)) {
         if (tile->wall) {
            // Check for nearby players
            auto nearbyPlayers = World::where<Player>([&](const Player& player) {
               return (std::abs(tile_x - player.tile_x) + std::abs(tile_y - player.tile_y) < 14);
            });
            if (!nearbyPlayers.empty()) {
               auto player = nearbyPlayers[0];
               World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", tile_x, tile_y)));
               audio().Bomb_Place.play();
               Character::move(tile_x - sign(player->tile_x - tile_x), tile_y);
               return Character::move(tile_x, tile_y - sign(player->tile_y - tile_y));
            }
            break;
         }
      };
   }
   return false;
}

void Bomber::update() {
   Character::update();
   tintColor.a = zeno(tintColor.a, 0.0, 0.1);
   if (health <= 0) {
      ShouldDestroy = true;
      audio().Enemy_Hurt.play();
   }
}

void Bomber::tickUpdate() {

   // Check for nearby players
   auto nearbyPlayers = World::where<Player>(
      [&](const Player& player) { return (std::abs(tile_x - player.tile_x) + std::abs(tile_y - player.tile_y) < 14); });

   // Check for nearby bombs
   auto nearbyBombs = World::where<Bomb>(
      [&](const Bomb& bomb) { return (std::abs(tile_x - bomb.tile_x) + std::abs(tile_y - bomb.tile_y) < 3); });
   auto nearbyBullets = World::where<Bullet>(
      [&](const Bullet& bullet) { return (std::abs(tile_x - bullet.tile_x) + std::abs(tile_y - bullet.tile_y) < 3); });

   // Move to player
   if (!nearbyBombs.empty()) {
      auto bomb = nearbyBombs[0];
      // Move away from bomb
      move(tile_x + (tile_x > bomb->tile_x ? 1 : -1), tile_y);
      move(tile_x, tile_y + (tile_y > bomb->tile_y ? 1 : -1));
   }
   if (!nearbyBullets.empty()) {
      auto bullet = nearbyBullets[0];
      // Move away from bullet

      if (bullet->direction_x + bullet->tile_x == tile_x && bullet->direction_y + bullet->tile_y == tile_y) {
         if (bullet->direction_x != 0) {
            move(tile_x, tile_y + (tile_y > bullet->tile_y ? 1 : -1));
         } else {
            move(tile_x + (tile_x > bullet->tile_x ? 1 : -1), tile_y);
         }
      }
   }

   else if (!nearbyPlayers.empty() && nearbyBombs.empty()) {
      auto player = nearbyPlayers[0];
      move(tile_x + sign(player->tile_x - tile_x), tile_y);
      move(tile_x, tile_y + sign(player->tile_y - tile_y));

      if (std::abs(tile_x - player->tile_x) + std::abs(tile_y - player->tile_y) < 2) {
         // Drop a bomb
         World::gameobjectstoadd.push_back(std::make_unique<Bomb>(Bomb("CoolBomb", tile_x, tile_y)));
         audio().Bomb_Place.play();

         // Move away from player after dropping bomb
         move(tile_x - sign(player->tile_x - tile_x), tile_y);
         move(tile_x, tile_y - sign(player->tile_y - tile_y));
      }
   }
}
