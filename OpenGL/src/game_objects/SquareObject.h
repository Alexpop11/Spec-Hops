#pragma once
#include "GameObject.h"

class SquareObject : public GameObject {
public:
   SquareObject() = default;
   SquareObject(const std::string& name, int drawPriority, int x, int y);
   virtual void render(Renderer& renderer) override;
   virtual void update() override;
   virtual void setUpShader(Renderer& renderer) override;
   float        r      = 0;
   float        g      = 0;
   float        b      = 0;
   int          tile_x = 0;
   int          tile_y = 0;
};
