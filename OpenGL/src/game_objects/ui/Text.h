#pragma once
#include "../GameObject.h"
#include "../../Input.h"
#include "../../Texture.h"
#include <glm/glm.hpp>
#include "imgui.h"

class Text : public GameObject {
public:
   Text(const std::string& text, ImFont* font, glm::vec2 position);
   virtual void render(Renderer& renderer) override;
   virtual void update() override;
   virtual void setUpShader(Renderer& renderer) override;

   ImFont* font;

protected:
   std::shared_ptr<Texture> texture;
};