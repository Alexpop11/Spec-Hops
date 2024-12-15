#pragma once
#include "../Character.h"
#include "../Player.h"
#include "../../World.h"
#include "../Bullet.h"
#include "TurretHead.h"
class LaserTurret : public Character {
public:
   Turret(const std::string& name, float x, float y);
   virtual void update() override;
   virtual void tickUpdate() override;
   int          aimDirection_x = 0;
   int          aimDirection_y = 0;
   int          bulletsToShoot = 0;
   bool         shot_last_tick = false;


   std::unique_ptr<TurretHead>      turretHead;
   virtual std::vector<GameObject*> children() override { return {turretHead.get()}; }
};
