

#include "GameObject.h"
#include "../Input.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


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

      shader->Bind();
      float currentTime = (float)glfwGetTime();
      shader->SetUniform1f("u_Time", currentTime);
      shader->SetUniform1f("u_StartTime", Input::startTime);

      // Calculate aspect ratio
      float aspectRatio = float(width) / float(height);

      // Set up orthographic projection matrix centered at (0,0)
      float     orthoWidth = Camera::scale * aspectRatio;
      glm::mat4 proj       = glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f,       // Left, Right
                                        -Camera::scale / 2.0f, Camera::scale / 2.0f, // Bottom, Top
                                        -1.0f, 1.0f);                                // Near, Far

      // Set up view matrix (camera transformation)
      glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-Camera::position, 0.0f));

      // Set up model matrix (object transformation)
      glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
      model           = glm::rotate(model, rotation * 3.14159f / 180.0f, glm::vec3(0, 0, 1));
      model           = glm::scale(model, glm::vec3(scale, scale, 1));

      // Calculate MVP matrix
      glm::mat4 mvp = proj * view * model;

      // Pass MVP matrix to the shader
      shader->SetUniformMat4f("u_MVP", mvp);

      // Pass resolution if needed
      shader->SetUniform2f("u_Resolution", {(float)width, (float)height});
   }
}


void GameObject::render(Renderer& renderer) {
   setUpShader(renderer);
}
