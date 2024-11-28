#include "Particles.h"
#include "../Input.h"
#include "../geometry/SceneGeometry.h"

#include <random>

Particles::Particles(const std::string& name, DrawPriority drawPriority, glm::vec2 position)
   : GameObject(name, drawPriority, position)
   , particles(std::vector<Particle>{
        Particle{position + glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)},
        Particle{position + glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)}
}),
   particleBuffer(
      std::make_shared<Buffer<Particle>>(particles,
                                         wgpu::bothBufferUsages(wgpu::BufferUsage::Vertex, wgpu::BufferUsage::CopyDst,
                                                                wgpu::BufferUsage::CopySrc, wgpu::BufferUsage::Storage),
                                         "Particles")),
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
   segmentBuffer(Buffer<Segment>(
      {}, wgpu::bothBufferUsages(wgpu::BufferUsage::CopySrc, wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Storage),
      "segments")),
   bvhBuffer(Buffer<BvhNode>(
      {}, wgpu::bothBufferUsages(wgpu::BufferUsage::CopySrc, wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Storage),
      "bvh")),
   vertexUniform(UniformBufferView<ParticleVertexUniform>::create(ParticleVertexUniform{VP()})),
   worldInfo(UniformBufferView<ParticleWorldInfo>::create(ParticleWorldInfo(0.0f, glm::vec2(0.0f)))) {}

void Particles::render(Renderer& renderer, RenderPass& renderPass) {
   if (particles.empty())
      return;

   // Update VP matrix
   this->vertexUniform.Update(ParticleVertexUniform{VP()});

   // Create bind group and draw
   BindGroup bindGroup = ParticleLayout::ToBindGroup(renderer.device, vertexUniform);
   renderPass.DrawInstanced(renderer.particles, *indexBuffer, bindGroup, {(uint32_t)vertexUniform.getOffset()},
                            particles.size(), *pointBuffer, *particleBuffer);
}

void Particles::pre_compute() {
   auto walls = SceneGeometry::computeWallPaths();
   segmentBuffer.upload(walls.bvh.segments);
   bvhBuffer.upload(walls.bvh.nodes);

   for (auto& segment : walls.bvh.segments) {
      Renderer::DebugLine(segment.start, segment.end, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
   }
}

void Particles::compute(Renderer& renderer, ComputePass& computePass) {
   worldInfo.Update(ParticleWorldInfo(Input::deltaTime, Renderer::MousePos()));
   BindGroup bindGroup =
      ParticleComputeLayout::ToBindGroup(renderer.device, std::forward_as_tuple(*particleBuffer, 0), worldInfo,
                                         std::forward_as_tuple(segmentBuffer, 0), std::forward_as_tuple(bvhBuffer, 0));
   computePass.dispatch(renderer.particlesCompute, bindGroup, {(uint32_t)worldInfo.getOffset()}, particles.size());
}

void Particles::update() {
   std::random_device rd;
   std::mt19937       gen(rd()); // Mersenne Twister generator

   // Define distribution from 0 to 1
   std::uniform_real_distribution<> vel_dist(-0.5, 0.5);
   std::uniform_real_distribution<> color_dist(0.0, 1.0);

   auto random_vel = glm::vec2(vel_dist(gen), vel_dist(gen)) * 2.0f;
   addParticle(position, random_vel, glm::vec4(color_dist(gen), color_dist(gen), color_dist(gen), 1.0f));
}

void Particles::addParticle(const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color) {
   particles.push_back({pos, vel, color});
   particleViews.push_back(particleBuffer->Add(particles.back()));
}
