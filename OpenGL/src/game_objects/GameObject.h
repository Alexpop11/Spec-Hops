#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <memory>
#include "glm/glm.hpp"

#include "../rendering/Renderer.h"

enum class DrawPriority {
   Background,
   Floor,
   Bomb,
   Character,
   Fog,
   UI,
};

class GameObject {
public:
   GameObject(const std::string& name, DrawPriority drawPriority, glm::vec2 position);
   GameObject(GameObject&& mE)            = default;
   GameObject& operator=(GameObject&& mE) = default;


   virtual ~GameObject() = default;
   bool ShouldDestroy    = false;

   virtual std::vector<GameObject*> children() { return {}; }

   virtual void render(Renderer& renderer);
   virtual void update();
   virtual void tickUpdate();

   std::string  name;
   DrawPriority drawPriority;
   glm::vec2    position;
   float        rotation = 0;
   float        scale    = 1.0f;

private:
   // Add any private members here if needed
};
