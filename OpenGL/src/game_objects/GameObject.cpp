

#include "GameObject.h"
#include "../Input.h"
#include "Camera.h"


GameObject::GameObject(const std::string& name, int drawPriority, float x, float y)
   : name(name)
   , drawPriority(drawPriority)
   , x(x)
   , y(y) {
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
      shader->SetUniform2f("u_Position", {x - Camera::x + 9, y - Camera::y + 9});
      shader->SetUniform2f("u_Resolution", {(float)width, (float)height});
   }
}

void GameObject::render(Renderer& renderer) {
   setUpShader(renderer);
}
