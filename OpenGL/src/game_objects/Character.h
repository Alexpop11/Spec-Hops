#pragma once
#include "SquareObject.h"
#include "../World.h"
#include "Tile.h"


class Character : public SquareObject {
public:
   Character(const std::string& name, int tile_x, int tile_y, std::string texturepath);
   virtual void move(int new_x, int new_y);
   virtual void update() override;
   virtual void tickUpdate() override;
   void         die();
   int          health = 1;
   virtual void        hurt();

   // Powerups
   bool hasBomb      = true;
   int  bombCoolDown = 0;

   bool hasBunnyHop      = true;
   int  bunnyHopCoolDown = 0;
   bool hoppedLastTurn   = false;
   bool hasGun           = true;
};

template <typename T>
int sign(T val) {
   return (T(0) < val) - (val < T(0));
}
