#pragma once
#include "SquareObject.h"
#include "Input.h"

class Tile : public SquareObject {
public:
   Tile(const std::string& name, bool wall, bool unbreakable, float x, float y);
   Tile(const std::string& name, float x, float y);
   virtual void update() override;
   virtual void explode();
   bool         wall;
   bool         unbreakable = false;


private:
   std::shared_ptr<Texture> wallTexture;
   std::shared_ptr<Texture> wallTextureUnbreakable;
   std::shared_ptr<Texture> floorTexture;
   void                     setTexture();
};
