#pragma once
#include "Character.h"

class Player : public Character {
public:
   Player() = default;
   Player(const std::string& name, int x, int y);
   virtual void update() override;
   bool         moved_last_tick = false;
   virtual void tickUpdate() override;
};
