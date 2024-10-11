#include "Renderer.h"
#include "Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "../game_objects/Camera.h"

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

Renderer::Renderer()
   : stars(starPipeline())
   , squareObject(squareObjectPipeline())
   , device(Application::get().getDevice()) {}


glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale) {
   glm::ivec2 windowSize = Application::get().windowSize();

   // Calculate aspect ratio
   float aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);

   // Create orthographic projection matrix
   float     orthoWidth = Camera::scale * aspectRatio;
   glm::mat4 projection =
      glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f, -Camera::scale / 2.0f, Camera::scale / 2.0f, -1.0f, 1.0f);

   // Create view matrix
   glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-Camera::position, 0.0f));

   // Create model matrix
   glm::mat4 model           = glm::translate(glm::mat4(1.0f), glm::vec3(objectPosition, 0.0f));
   float     rotationRadians = glm::radians(objectRotationDegrees);
   model                     = glm::rotate(model, rotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
   model                     = glm::scale(model, glm::vec3(objectScale, objectScale, 1.0f));

   // Combine matrices to form MVP
   glm::mat4 mvp = projection * view * model;

   return mvp;
}
