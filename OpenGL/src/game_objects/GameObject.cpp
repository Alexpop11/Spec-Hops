

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

void GameObject::compute(Renderer& renderer, ComputePass& computePass) {}
