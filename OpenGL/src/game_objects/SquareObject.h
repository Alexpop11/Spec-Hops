#pragma once
#include "GameObject.h"
#include "../rendering/Renderer.h"
#include "../rendering/Texture.h"
#include <glm/glm.hpp>

class SquareObject : public GameObject {
public:
   SquareObject(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y, std::string texturePath);
   virtual void render(Renderer& renderer, RenderPass& renderPass) override;
   virtual void update() override;
   glm::vec4    tintColor = glm::vec4(0.0f);
   float        opacity   = 1;

   void       setTile(glm::ivec2 position) { tilePosition = position; }
   glm::ivec2 getTile() const { return tilePosition; }

private:
   glm::ivec2 tilePosition;

   std::shared_ptr<Buffer<SquareObjectVertex>> pointBuffer;
   std::shared_ptr<IndexBuffer>                indexBuffer;
   BufferView<SquareObjectVertexUniform>       vertexUniform;
   BufferView<SquareObjectFragmentUniform>     fragmentUniform;

protected:
   std::shared_ptr<Texture> texture;
};
