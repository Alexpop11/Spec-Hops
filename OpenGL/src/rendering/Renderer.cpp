#include "Renderer.h"
#include "Application.h"

RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>> pipeline() {
   auto& application   = Application::get();
   auto  device        = application.getDevice();
   auto  surfaceFormat = application.getSurfaceFormat();

   Shader             shader(device, "/Users/andrepopovitch/coding/SpaceBoom/OpenGL/res/shaders/stars.wgsl");
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   return RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>>(
      "Stars", device, shader, wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

Renderer::Renderer()
   : stars(pipeline()) {}
