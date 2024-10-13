#pragma once

#include <iostream>
#include <string>
#include <optional>
#include <memory>
#include "glm/glm.hpp"
#include "../Generator.h"

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

   // Add coroutine
   void addCoroutine(Generator<> coroutine) { coroutines.emplace_back(std::move(coroutine)); }

   // Run coroutines
   void progressCoroutines() {
      for (auto it = coroutines.begin(); it != coroutines.end();) {
         auto& coroutine = *it;

         // Check if the coroutine is waiting for frames
         auto& wait = coroutine.wait_for_frames();
         if (auto wf = std::get_if<int>(&wait)) {
            std::cout << "Waiting for " << *wf << " frames\n";
            if (*wf > 0) {
               (*wf)--;
               ++it;
               continue;
            }
         }

         // Progress the coroutine
         if (!coroutine.move_next()) {
            // Coroutine finished, remove it
            it = coroutines.erase(it);
         } else {
            ++it;
         }
      }
   }

   // Store active coroutines
   std::vector<Generator<>> coroutines;

private:
   // Add any private members here if needed
};
