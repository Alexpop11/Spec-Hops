#pragma once
#include "../Character.h"
#include "../Player.h"
#include "../../World.h"
#include "../entities/Bomb.h"

class Bomber : public Character {
public:
   Bomber(const std::string& name, float x, float y);
   virtual void update() override;
   virtual void tickUpdate() override;
   virtual void move(int new_x, int new_y) override;
};
