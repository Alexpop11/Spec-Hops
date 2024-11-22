#include "Particles.h"
#include "../Input.h"
#include "../rendering/Texture.h"

Particles::Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position)
   : GameObject(name, drawPriority, {position.x, position.y}) {}

void Particles::render(Renderer& renderer, RenderPass& renderPass) {}

void Particles::update() {}

void Particles::compute(Renderer& renderer, ComputePass& computePass) {}
