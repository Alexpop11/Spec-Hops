#include "Renderer.h"
#include "Application.h"

RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>> starPipeline() {
   auto& application   = Application::get();
   auto  device        = application.getDevice();
   auto  surfaceFormat = application.getSurfaceFormat();

   Shader             shader(device, "/Users/andrepopovitch/coding/SpaceBoom/OpenGL/res/shaders/stars.wgsl");
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   return RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>>(
      "Stars", device, shader, wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

RenderPipeline<BindGroupLayouts<SquareObjectUniformLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>
squareObjectPipeline() {
   auto& application   = Application::get();
   auto  device        = application.getDevice();
   auto  surfaceFormat = application.getSurfaceFormat();

   Shader             shader(device, "/Users/andrepopovitch/coding/SpaceBoom/OpenGL/res/shaders/square_object.wgsl");
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   return RenderPipeline<BindGroupLayouts<SquareObjectUniformLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>(
      "Stars", device, shader, wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

Renderer::Renderer()
   : stars(starPipeline())
   , squareObject(squareObjectPipeline()) {}
