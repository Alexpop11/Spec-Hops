#pragma once
#include "SquareObject.h"
#include "../World.h"


class Entity : public SquareObject {
public:
   Entity(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y, std::string texturepath);
   virtual void kick(bool hitWall, int dx, int dy);
};
