#pragma once
#include "GameObject.h"
#include "../rendering/Renderer.h"
#include "../rendering/Texture.h"
#include <glm/glm.hpp>

class Particles : public GameObject {
public:
   Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position);
   virtual void render(Renderer& renderer, RenderPass& renderPass) override;
   virtual void update() override;
   virtual void compute(Renderer& renderer, ComputePass& computePass) override;
   glm::vec4    tintColor = glm::vec4(0.0f);

private:
protected:
};
