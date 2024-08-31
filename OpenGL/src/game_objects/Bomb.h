#pragma once
#include "SquareObject.h"

class Bomb : public SquareObject {
public:
   Bomb() = default;
   Bomb(const std::string& name, float x, float y);
   virtual void tickUpdate() override;
   int          ExplodeTick;
};
