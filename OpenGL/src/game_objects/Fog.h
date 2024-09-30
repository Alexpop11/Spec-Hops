#pragma once
#include "../World.h"
#include "GameObject.h"
#include "../Texture.h"

class Fog : public GameObject {
public:
   Fog();
   virtual void render(Renderer& renderer) override;
   virtual void update() override;
   virtual void setUpShader(Renderer& renderer) override;

protected:
   std::shared_ptr<Texture> texture;
};
