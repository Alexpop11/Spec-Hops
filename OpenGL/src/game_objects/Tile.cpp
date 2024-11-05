#include "Tile.h"

Tile::Tile(const std::string& name, bool wall, bool unbreakable, float x, float y)
   : SquareObject(name, wall ? DrawPriority::Wall : DrawPriority::Floor, x, y, "alt-wall-bright.png")
   , wall(wall)
   , unbreakable(unbreakable)
   , wallTexture(Texture::create("alt-wall-bright.png"))
   , wallTextureUnbreakable(Texture::create("alt-wall-unbreakable.png"))
   , floorTexture(Texture::create(std::vector<std::string>{"2-alt-floor.png", "2-alt-floor-2.png"}[rand() % 2])) {
   setTexture();

}

Tile::Tile(const std::string& name, float x, float y)
   : Tile(name, false, true, x, y) {}

void Tile::explode() {
   if (!unbreakable || !wall) {
      tintColor = {0.8, 0.5, 0.5, 0.9};
      wall      = false;
   }
}

std::vector<glm::vec2> Tile::getBounds() {
   if (wall) {
      std::vector<glm::vec2> bounds = {
         position + glm::vec2{-0.5, -0.5},
         position + glm::vec2{0.5,  -0.5},
         position + glm::vec2{0.5,  0.5 },
         position + glm::vec2{-0.5, 0.5 },
      };
      return bounds;
   } else {
      return std::vector<glm::vec2>{};
   }
}


void Tile::update() {
   tintColor.a = zeno(tintColor.a, 0, 0.1);
   setTexture();
}

void Tile::setTexture() {
   if (wall) {
      if (unbreakable) {
         texture = wallTextureUnbreakable;
      } else {
         texture = wallTexture;
      }
   } else {
      texture = floorTexture;
   }
}
