#pragma once
#include "GameObject.h"
#include "../rendering/Renderer.h"
#include "../rendering/Texture.h"
#include <glm/glm.hpp>

class Particles : public GameObject {
public:
   Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position);
   virtual void render(Renderer& renderer, RenderPass& renderPass) override;
   virtual void update() override;

   void addParticle(const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color);

private:
   std::vector<Particle>                    particles;
   std::vector<BufferView<Particle, false>> particleViews;
   std::shared_ptr<Buffer<Particle>>        particleBuffer;
   std::shared_ptr<Buffer<ParticleVertex>>  pointBuffer;
   std::shared_ptr<IndexBuffer>             indexBuffer;
   UniformBufferView<ParticleVertexUniform> vertexUniform;

private:
protected:
};
