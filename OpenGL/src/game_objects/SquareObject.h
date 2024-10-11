#pragma once
#include "GameObject.h"
#include "../rendering/Renderer.h"
#include "../rendering/Texture.h"
#include <glm/glm.hpp>

class SquareObject : public GameObject {
public:
   SquareObject(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y, std::string texturePath);
   virtual void render(Renderer& renderer) override;
   virtual void update() override;
   glm::vec4    tintColor = glm::vec4(0.0f);
   int          tile_x    = 0;
   int          tile_y    = 0;

private:
   Buffer<SquareObjectVertex>              pointBuffer;
   IndexBuffer                             indexBuffer;
   BufferView<SquareObjectVertexUniform>   vertexUniform;
   BufferView<SquareObjectFragmentUniform> fragmentUniform;
   Texture                                 texture;
};
