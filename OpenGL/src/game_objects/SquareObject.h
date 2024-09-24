#pragma once
#include "GameObject.h"
#include "Input.h"
#include "../Texture.h"
#include <glm/glm.hpp>

class SquareObject : public GameObject {
public:
   SquareObject() = default;
   SquareObject(const std::string& name, int drawPriority, int x, int y, std::string texturepath);
   virtual void             render(Renderer& renderer) override;
   virtual void             update() override;
   virtual void             setUpShader(Renderer& renderer) override;
   glm::vec4                tintColor = glm::vec4(0.0f);
   std::shared_ptr<Texture> texture;
   int                      tile_x = 0;
   int                      tile_y = 0;
};
