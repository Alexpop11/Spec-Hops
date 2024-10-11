#include "Renderer.h"
#include "Application.h"

RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>> starPipeline() {
   auto& application   = Application::get();
   auto  device        = application.getDevice();
   auto  surfaceFormat = application.getSurfaceFormat();

   Shader             shader(device, "stars.wgsl");
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   return RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>>(
      "Stars", device, shader, wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayout<glm::vec2, glm::vec2>> squareObjectPipeline() {
   auto& application   = Application::get();
   auto  device        = application.getDevice();
   auto  surfaceFormat = application.getSurfaceFormat();

   Shader             shader(device, "square_object.wgsl");
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   return RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>(
      "Stars", device, shader, wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

Texture createTexture() {
   auto& application = Application::get();
   auto  device      = application.getDevice();
   auto  queue       = application.getQueue();
   return Texture(device, queue, "/Users/andrepopovitch/coding/SpaceBoom/OpenGL/res/textures/alternate-player.png");
}

Renderer::Renderer()
   : stars(starPipeline())
   , squareObject(squareObjectPipeline())
   , player(createTexture()) {}
