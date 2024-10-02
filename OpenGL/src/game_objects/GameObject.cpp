

#include "GameObject.h"
#include "../Input.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


GameObject::GameObject(const std::string& name, DrawPriority drawPriority, glm::vec2 position)
   : name(name)
   , drawPriority(drawPriority)
   , position(position) {
   // TODO: Add any additional initialization if needed
}

void GameObject::update() {}

void GameObject::tickUpdate() {}

void GameObject::setUpShader(Renderer& renderer) {
   if (shader) {
      shader->Bind();
      float currentTime = (float)glfwGetTime();
      shader->SetUniform1f("u_Time", currentTime);
      shader->SetUniform1f("u_StartTime", Input::startTime);

      auto mvp = CalculateMVP(renderer.WindowSize(), position, rotation, scale);

      // Pass MVP matrix to the shader
      shader->SetUniformMat4f("u_MVP", mvp);
   }
}


void GameObject::render(Renderer& renderer) {
   setUpShader(renderer);
}
