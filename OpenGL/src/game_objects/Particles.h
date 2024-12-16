#pragma once
#include "GameObject.h"
#include "../geometry/BVH.h"
#include "../rendering/Renderer.h"
#include "../rendering/Texture.h"
#include <glm/glm.hpp>

class Particles : public GameObject {
public:
   Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position, size_t particleCount);
   virtual void render(Renderer& renderer, RenderPass& renderPass) override;
   virtual void update() override;
   virtual void pre_compute() override;
   virtual void compute(Renderer& renderer, ComputePass& computePass) override;
   void         addParticle(const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color);

private:
   std::vector<Particle>                    particles;
   std::vector<BufferView<Particle, false>> particleViews;
   std::shared_ptr<Buffer<Particle>>        particleBuffer;
   std::shared_ptr<Buffer<ParticleVertex>>  pointBuffer;
   std::shared_ptr<IndexBuffer>             indexBuffer;
   Buffer<Segment>                          segmentBuffer;
   Buffer<BvhNode>                          bvhBuffer;
   UniformBufferView<ParticleVertexUniform> vertexUniform;
   UniformBufferView<ParticleWorldInfo>     worldInfo;
   size_t                                   particleCount;

private:
protected:
};
