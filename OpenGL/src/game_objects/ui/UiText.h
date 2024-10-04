#pragma once
#include "../GameObject.h"
#include "../../Input.h"
#include "../../Texture.h"
#include <glm/glm.hpp>

class UiText : public GameObject {
public:
   UiText(const std::string& text, glm::vec2 position);
   virtual void render(Renderer& renderer) override;
   virtual void update() override;
   virtual void setUpShader(Renderer& renderer) override;

protected:
   std::shared_ptr<Texture> texture;
};
