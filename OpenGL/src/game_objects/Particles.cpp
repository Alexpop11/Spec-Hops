#include "Particles.h"
#include "../Input.h"

Particles::Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position)
   : GameObject(name, drawPriority, position)
   , particles(std::vector<Particle>{
        Particle{position, glm::vec2(0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}
}),
   particleBuffer(
      Buffer<Particle>(particles, wgpu::bothBufferUsages(wgpu::BufferUsage::Vertex, wgpu::BufferUsage::CopyDst,
                                                         wgpu::BufferUsage::CopySrc))),
   pointBuffer(Buffer<ParticleVertex>::create(
      {
         ParticleVertex{glm::vec2(-0.5f, -0.5f) * (1.0f / 16.0f)}, // 0
         ParticleVertex{glm::vec2(0.5f, -0.5f) * (1.0f / 16.0f)},  // 1
         ParticleVertex{glm::vec2(0.5f, 0.5f) * (1.0f / 16.0f)},   // 2
         ParticleVertex{glm::vec2(-0.5f, 0.5f) * (1.0f / 16.0f)},  // 3
      },
      wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Vertex))),
   indexBuffer(IndexBuffer::create(
      {
         0, 1, 2, // Triangle #0 connects points #0, #1 and #2
         0, 2, 3  // Triangle #1 connects points #0, #2 and #3
      },
      wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Index))),
   vertexUniform(UniformBufferView<ParticleVertexUniform>::create(ParticleVertexUniform{MVP()})) {}

void Particles::render(Renderer& renderer, RenderPass& renderPass) {
   if (particles.empty())
      return;
   // particleBuffer.upload(particles);

   // Update MVP matrix
   this->vertexUniform.Update(ParticleVertexUniform{MVP()});

   // Create bind group and draw
   BindGroup bindGroup = ParticleLayout::ToBindGroup(renderer.device, vertexUniform);
   //renderPass.Draw(renderer.particles, *pointBuffer, *indexBuffer, bindGroup, {(uint32_t)vertexUniform.getOffset()});
}

void Particles::update() {
   for (auto& particle : particles) {
      particle.position += particle.velocity * Input::deltaTime;
   }
   addParticle(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void Particles::addParticle(const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color) {
   // particles.push_back({pos, vel, color});
   // particleBuffer.Add(particles.back());
}
