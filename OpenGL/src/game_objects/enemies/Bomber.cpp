#include "Bomber.h"
#include "../../AudioEngine.h"

Bomber::Bomber(const std::string& name, float x, float y)
   : Character(name, x, y, "enemy.png") {
   drawPriority = DrawPriority::Character;
   health       = 1;
}

bool Bomber::move(int new_x, int new_y) {

   auto nearbyBombsCurrent = World::where<Bomb>([&](const Bomb& bomb) {
      return (std::abs(getTile().x - bomb.getTile().x) + std::abs(getTile().y - bomb.getTile().y) < 3);
   });
   auto nearbyBombsNew     = World::where<Bomb>(
      [&](const Bomb& bomb) { return (std::abs(new_x - bomb.getTile().x) + std::abs(new_y - bomb.getTile().y) < 3); });
   if (nearbyBombsNew.empty() || !nearbyBombsCurrent.empty()) {
      Character::move(new_x, new_y);
   }
   if (nearbyBombsCurrent.empty()) {
      for (auto& tile : World::at<Tile>(new_x, new_y)) {
         if (tile->wall) {
            // Check for nearby players
            auto nearbyPlayers = World::where<Player>([&](const Player& player) {
               return (std::abs(getTile().x - player.getTile().x) + std::abs(getTile().y - player.getTile().y) < 14);
            });
            if (!nearbyPlayers.empty()) {
               auto player = nearbyPlayers[0];
               World::gameobjectstoadd.push_back(std::make_unique<Bomb>("CoolBomb", getTile().x, getTile().y));
               audio().Bomb_Place.play();
               Character::move(getTile().x - sign(player->getTile().x - getTile().x), getTile().y);
               return Character::move(getTile().x, getTile().y - sign(player->getTile().y - getTile().y));
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
      [&](const Player& player) { return (std::abs(getTile().x - player.getTile().x) + std::abs(getTile().y - player.getTile().y) < 14); });

   // Check for nearby bombs
   auto nearbyBombs = World::where<Bomb>(
      [&](const Bomb& bomb) { return (std::abs(getTile().x - bomb.getTile().x) + std::abs(getTile().y - bomb.getTile().y) < 3); });
   auto nearbyBullets = World::where<Bullet>(
      [&](const Bullet& bullet) { return (std::abs(getTile().x - bullet.getTile().x) + std::abs(getTile().y - bullet.getTile().y) < 3); });

   // Move to player
   if (!nearbyBombs.empty()) {
      auto bomb = nearbyBombs[0];
      // Move away from bomb
      move(getTile().x + (getTile().x > bomb->getTile().x ? 1 : -1), getTile().y);
      move(getTile().x, getTile().y + (getTile().y > bomb->getTile().y ? 1 : -1));
   }
   if (!nearbyBullets.empty()) {
      auto bullet = nearbyBullets[0];
      // Move away from bullet

      if (bullet->direction_x + bullet->getTile().x == getTile().x && bullet->direction_y + bullet->getTile().y == getTile().y) {
         if (bullet->direction_x != 0) {
            move(getTile().x, getTile().y + (getTile().y > bullet->getTile().y ? 1 : -1));
         } else {
            move(getTile().x + (getTile().x > bullet->getTile().x ? 1 : -1), getTile().y);
         }
      }
   }

   else if (!nearbyPlayers.empty() && nearbyBombs.empty()) {
      auto player = nearbyPlayers[0];
      move(getTile().x + sign(player->getTile().x - getTile().x), getTile().y);
      move(getTile().x, getTile().y + sign(player->getTile().y - getTile().y));

      if (std::abs(getTile().x - player->getTile().x) + std::abs(getTile().y - player->getTile().y) < 2) {
         // Drop a bomb
         World::gameobjectstoadd.push_back(std::make_unique<Bomb>("CoolBomb", getTile().x, getTile().y));
         audio().Bomb_Place.play();

         // Move away from player after dropping bomb
         move(getTile().x - sign(player->getTile().x - getTile().x), getTile().y);
         move(getTile().x, getTile().y - sign(player->getTile().y - getTile().y));
      }
   }
}
