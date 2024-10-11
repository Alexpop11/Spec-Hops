#pragma once
#include "GameObject.h"
#include "../rendering/DataFormats.h"

class Background : public GameObject {
public:
   Background(const std::string& name);

   virtual void render(Renderer& renderer) override;
   virtual void update() override;

private:
   Buffer<glm::vec2>           pointBuffer;
   Buffer<uint16_t>            indexBuffer;
   UniformBuffer<StarUniforms> uniformBuffer;
};
