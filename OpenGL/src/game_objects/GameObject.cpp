

#include "GameObject.h"
#include "../Input.h"
#include "Camera.h"
#include "glm/glm.hpp"


GameObject::GameObject(const std::string& name, int drawPriority, glm::vec2 position)
   : name(name)
   , drawPriority(drawPriority)
   , position(position) {
   // TODO: Add any additional initialization if needed
}

void GameObject::update() {}

void GameObject::tickUpdate() {}

void GameObject::setUpShader(Renderer& renderer) {
   if (shader) {
      auto [width, height] = renderer.WindowSize();

      /* Render here */
      shader->Bind();
      float currentTime = (float)glfwGetTime();
      shader->SetUniform1f("u_Time", currentTime);
      shader->SetUniform1f("u_StartTime", Input::startTime);
      shader->SetUniform1f("u_AspectRatio", float(width) / float(height));
      shader->SetUniform2f("u_Position", position - Camera::position + glm::vec2{9.0, 9.0});
      shader->SetUniform2f("u_Resolution", {(float)width, (float)height});
   }
}

void GameObject::render(Renderer& renderer) {
   setUpShader(renderer);
}
