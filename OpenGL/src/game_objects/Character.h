#pragma once
#include "Entity.h"
#include "../World.h"
#include "Tile.h"

struct KickState {
   std::weak_ptr<Entity> victim;
   glm::ivec2            direction;
   bool                  intoWall;
};

class Character : public Entity {
public:
   Character(const std::string& name, int tile_x, int tile_y, std::string texturepath);
   virtual bool             move(int new_x, int new_y);
   virtual void             update() override;
   virtual void             tickUpdate() override;
   void                     die();
   int                      health = 1;
   virtual void             hurt();
   virtual void             kick(bool hitWall, int dx, int dy) override;
   int                      stunnedLength = 0;
   std::optional<KickState> kicking;

   // Powerups
   bool hasBomb      = true;
   int  bombCoolDown = 0;

   bool hasBunnyHop      = true;
   int  bunnyHopCoolDown = 0;
   bool hoppedLastTurn   = false;
   bool hasGun           = true;
   int  gunCooldown      = 0;
};

template <typename T>
int sign(T val) {
   return (T(0) < val) - (val < T(0));
}
