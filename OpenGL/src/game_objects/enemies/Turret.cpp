#include "Turret.h"

Turret::Turret(const std::string& name, float x, float y)
   : Character(name, x, y) {
   drawPriority = 3;
   health       = 1;
   r            = 0.8;
   g            = 0.4;
   b            = 0.2;
}

void Turret::update() {
   Character::update();
   if (health <= 0) {
      ShouldDestroy = true;
   }
}

void Turret::tickUpdate() {
   if (ticksBeforeNextSpray > 0) {
      ticksBeforeNextSpray--;
      return;
   }
      if (shot_last_tick) {
         shot_last_tick = false;
         return;
      }
      shot_last_tick     = true;
      auto nearbyPlayers = World::where<Player>([&](const Player& player) {
         // Check for horizontal proximity
         if (std::abs(tile_x - player.tile_x) <= 10 && tile_y == player.tile_y) {
            aimDirection_x = (player.tile_x > tile_x) ? 1 : -1;
            aimDirection_y = 0;
            bulletsToShoot = 3;
            return true;
         }
         // Check for vertical proximity
         if (std::abs(tile_y - player.tile_y) <= 10 && tile_x == player.tile_x) {
            aimDirection_y = (player.tile_y > tile_y) ? 1 : -1;
            aimDirection_x = 0;
            bulletsToShoot = 3;
            return true;
         }
         return false;
      });

      // Shoot a bullet if a player is detected
      if (bulletsToShoot >= 1) {
         World::gameobjectstoadd.push_back(std::make_unique<Bullet>(
            Bullet("CoolBullet", tile_x + aimDirection_x, tile_y + aimDirection_y, aimDirection_x, aimDirection_y)));
         bulletsToShoot -= 1;
      } else {
         aimDirection_x       = 0;
         aimDirection_y       = 0;
         ticksBeforeNextSpray = 9;
      }
}