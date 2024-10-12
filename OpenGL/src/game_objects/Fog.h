#pragma once
#include "../World.h"
#include "GameObject.h"
#include "clipper2/clipper.h"

class Fog : public GameObject {
public:
   Fog();
   virtual void render(Renderer& renderer) override;
   virtual void update() override;

   glm::vec4 mainFogColor;
   glm::vec4 tintFogColor;

private:
   virtual void renderPolyTree(Renderer& renderer, const Clipper2Lib::PolyTreeD& polytree, glm::vec4 color,
                               glm::vec4 bandColor) const;
};
