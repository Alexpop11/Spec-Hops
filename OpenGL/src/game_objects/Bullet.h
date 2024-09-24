#pragma once
#include "SquareObject.h"

class Bullet : public SquareObject {
public:
   Bullet(const std::string& name, float x, float y, int direction_x, int direction_y);
   virtual void tickUpdate() override;
   int          direction_x;
   int          direction_y;
};
