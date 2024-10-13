#include "Entity.h"
#include "Input.h"
#include "../AudioEngine.h"


Entity::Entity(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y, std::string texturepath)
   : SquareObject(name, drawPriority, tile_x, tile_y, texturepath) {}

void Entity::kick(bool hitWall, int dx, int dy) {
   audio().Impact.play();
   tile_x += dx;
   tile_y += dy;
}
