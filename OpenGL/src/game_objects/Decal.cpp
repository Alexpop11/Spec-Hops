#include "Player.h"
#include "Decal.h"
#include "../World.h"
#include "Tile.h"
#include "AudioEngine.h""

Decal::Decal(const std::string& name, float x, float y, const std::string& type)
   : SquareObject(name, DrawPriority::Decal, x, y, chooseTexture(type))
   , texturepath(chooseTexture(type)) {
   if (texturepath == "explosion-decal.png") {
      scale    = std::vector<float>{0.85, 0.9, 0.95, 1, 1.05}[rand() % 5];
   }
   else if (texturepath == "crater-decal.png") {
      scale = 5;
   }
   rotation  = std::vector<int>{0, 90, 180, 270}[rand() % 4];
   tintColor = {0.8, 0.5, 0.5, 0.9};
    
}

//std::string createDecal(std::string decalType, float x, float y) {
//   if (decalType == "explosion") {
//      World::gameobjectstoadd.push_back(std::make_unique<Decal>("ExplosionDecal", x, y, "explosion-decal.png"));
//   }
//}
//
//std::string createDecal(std::string decalType) {
//   return createDecal(decalType, getTile().x, getTile().y);
//}


std::string Decal::chooseTexture(const std::string& type) {
   if (type == "explosion") {
      return "explosion-decal.png";
   } else if (type == "crater") {
      return "crater-decal.png";
   }
   return "enemy.png"; // Fallback path if type doesn't match any condition
}

void Decal::tickUpdate() {
   tintColor.a = zeno(tintColor.a, 0.0, 1);
}

void Decal::fade() {
   ShouldDestroy = true;
}