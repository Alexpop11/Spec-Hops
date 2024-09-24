#pragma once
#include "SquareObject.h"
#include "Bomb.h"

class Mine : public Bomb {
public:
   Mine(const std::string& name, float x, float y);
   virtual void tickUpdate() override;
   bool         red_last_frame    = false;
   bool         detectedCharacter = false;
};
