#include "Particles.h"
#include "../Input.h"

Particles::Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position)
   : GameObject(name, drawPriority, position)
   , particleBuffer(
        Buffer<Particle>({}, wgpu::bothBufferUsages(wgpu::BufferUsage::Vertex, wgpu::BufferUsage::CopyDst)))
   , pointBuffer(Buffer<ParticleVertex>::create(
        {
           ParticleVertex{glm::vec2(-0.5f, -0.5f)}, // 0
           ParticleVertex{glm::vec2(0.5f, -0.5f)},  // 1
           ParticleVertex{glm::vec2(0.5f, 0.5f)},   // 2
           ParticleVertex{glm::vec2(-0.5f, 0.5f)},  // 3
        },
        wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Vertex)))
   , indexBuffer(IndexBuffer::create(
        {
           0, 1, 2, // Triangle #0 connects points #0, #1 and #2
           0, 2, 3  // Triangle #1 connects points #0, #2 and #3
        },
        wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Index)))
   , vertexUniform(
        UniformBuffer<ParticleVertexUniform>({ParticleVertexUniform{glm::mat4(1.0f)}}, wgpu::BufferUsage::CopyDst)) {}

void Particles::render(Renderer& renderer, RenderPass& renderPass) {
   if (particles.empty())
      return;

   // Update MVP matrix
   vertexUniform.upload({ParticleVertexUniform{CalculateMVP(position, rotation, scale)}});

   // Create bind group and draw
   BindGroup bindGroup = ParticleLayout::ToBindGroup(renderer.device, std::forward_as_tuple(vertexUniform, 0));
   renderPass.Draw(renderer.particles, *pointBuffer, *indexBuffer, bindGroup, {});
}

void Particles::update() {
   for (auto& particle : particles) {
      particle.position += particle.velocity * Input::deltaTime;
   }
}

void Particles::addParticle(const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color) {
   particles.push_back({pos, vel, color});
}
