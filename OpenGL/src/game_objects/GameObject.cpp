

#include "GameObject.h"
#include "../Input.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


GameObject::GameObject(const std::string& name, DrawPriority drawPriority, glm::vec2 position)
   : name(name)
   , drawPriority(drawPriority)
   , position(position) {}

void GameObject::update() {}

void GameObject::tickUpdate() {}

void GameObject::render(Renderer& renderer, RenderPass& renderPass) {
   std::cout << "Rendering GameObject (you should not see this lol) " << name << std::endl;
}

void GameObject::pre_compute() {}
void GameObject::compute(Renderer& renderer, ComputePass& computePass) {}

glm::mat4 GameObject::getLocalTransform() const {
   return CalculateModel(position, rotation, scale);
}

glm::mat4 GameObject::MVP() const {
   glm::mat4 localTransform = getLocalTransform();

   if (parent) {
      // Get parent's transform and combine with local transform
      glm::mat4 parentMVP = parent->MVP();
      return parentMVP * localTransform;
   }

   // No parent, just apply view and projection to local transform
   glm::mat4 view       = CalculateView();
   glm::mat4 projection = CalculateProjection();
   return projection * view * localTransform;
}

glm::mat4 GameObject::VP() const {
   return CalculateProjection() * CalculateView();
}
