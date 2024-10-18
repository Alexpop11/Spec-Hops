#include "Turret.h"
#include "../../AudioEngine.h"

Turret::Turret(const std::string& name, float x, float y)
   : Character(name, x, y, "enemy.png") {
   drawPriority = DrawPriority::Character;
   health       = 1;
}

void Turret::update() {
   Character::update();
   tintColor.a = zeno(tintColor.a, 0.0, 0.5);
   if (health <= 0) {
      ShouldDestroy = true;
   }
}

void Turret::tickUpdate() {
   if (stunnedLength == 0) {
      if (shot_last_tick) {
         shot_last_tick = false;
         return;
      }
      shot_last_tick = true;

      // Find nearby players without modifying the turret's state
      auto nearbyPlayers = World::where<Player>([&](const Player& player) -> bool {
         // Check horizontal proximity
         if (std::abs(getTile().x - player.getTile().x) <= 10 && getTile().y == player.getTile().y) {
            return true;
         }
         // Check vertical proximity
         if (std::abs(getTile().y - player.getTile().y) <= 10 && getTile().x == player.getTile().x) {
            return true;
         }
         return false;
      });

      bool playerDetected = false;

      // Process nearby players to set aim direction and bullets to shoot
      for (const auto& playerPtr : nearbyPlayers) {
         const Player& player = *playerPtr;

         // Check horizontal proximity
         if (std::abs(getTile().x - player.getTile().x) <= 10 && getTile().y == player.getTile().y) {
            aimDirection_x = (player.getTile().x > getTile().x) ? 1 : -1;
            aimDirection_y = 0;
            bulletsToShoot = 3;
            playerDetected = true;
            break; // Assuming you handle one player at a time
         }

         // Check vertical proximity
         if (std::abs(getTile().y - player.getTile().y) <= 10 && getTile().x == player.getTile().x) {
            aimDirection_y = (player.getTile().y > getTile().y) ? 1 : -1;
            aimDirection_x = 0;
            bulletsToShoot = 3;
            playerDetected = true;
            break; // Assuming you handle one player at a time
         }
      }

      // If a player was detected, shoot a bullet
      if (bulletsToShoot >= 1) {
         audio().Bullet_Sound.play();
         World::gameobjectstoadd.push_back(std::make_unique<Bullet>(
            "CoolBullet", getTile().x + aimDirection_x, getTile().y + aimDirection_y, aimDirection_x, aimDirection_y));
         bulletsToShoot -= 1;
      } else {
         // Reset aim direction if no players are detected or no bullets left to shoot
         aimDirection_x = 0;
         aimDirection_y = 0;
      }
   } else {
      stunnedLength--;
   }
}
