#pragma once
#include "../World.h"
#include "GameObject.h"
#include "clipper2/clipper.h"

class Fog : public GameObject {
public:
   Fog();
   virtual void render(Renderer& renderer) override;
   virtual void update() override;
   virtual void setUpShader(Renderer& renderer) override;

private:
   virtual void renderPolyTree(Renderer& renderer, const Clipper2Lib::PolyTreeD& polytree, glm::vec4 color) const;
};
