#pragma once
#include "SquareObject.h"
#include "../World.h"
#include "Tile.h"


class Character : public SquareObject {
public:
   Character() = default;
   Character(const std::string& name, int x, int y);
   virtual void move(int new_x, int new_y);
   virtual void update() override;
   virtual void tickUpdate() override;
   void         die();
   int          health = 1;

   // Powerups
   bool hasBomb      = true;
   int  bombCoolDown = 0;

   bool hasBunnyHop      = true;
   int  bunnyHopCoolDown = 0;
   bool hoppedLastTurn   = false;
};

template <typename T>
int sign(T val) {
   return (T(0) < val) - (val < T(0));
}