#pragma once
#include "SquareObject.h"

class Decal : public SquareObject {
public:
   Decal(const std::string& name, float x, float y, const std::string& type);
   virtual void tickUpdate() override;
   void         fade();
   //static void  createDecal(std::string decalType, float x, float y);
private:
   std::string chooseTexture(const std::string& type);
   std::string texturepath;
};