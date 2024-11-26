#include "Renderer.h"
#include "Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "../game_objects/Camera.h"

Renderer::Renderer()
   : stars(RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>,
                          VertexBufferLayouts<VertexBufferLayout<glm::vec2>>>("stars.wgsl"))
   , squareObject(RenderPipeline<BindGroupLayouts<SquareObjectLayout>,
                                 VertexBufferLayouts<VertexBufferLayout<glm::vec2, glm::vec2>>>("square_object.wgsl"))
   , line(
        RenderPipeline<BindGroupLayouts<LineLayout>, VertexBufferLayouts<VertexBufferLayout<LineVertex>>>("line.wgsl"))
   , fog(RenderPipeline<BindGroupLayouts<FogLayout>, VertexBufferLayouts<VertexBufferLayout<FogVertex>>>("fog.wgsl"))
   , particles(
        RenderPipeline<
           BindGroupLayouts<ParticleLayout>,
           VertexBufferLayouts<VertexBufferLayout<glm::vec2>, InstanceBufferLayout<glm::vec2, glm::vec2, glm::vec4>>>(
           "particles.wgsl"))
   , particlesCompute(ComputePipeline<BindGroupLayouts<ParticleComputeLayout>>("particlesCompute.wgsl"))
   , device(Application::get().getDevice())
   , linePoints(
        std::vector<LineVertex>{
           LineVertex{0.0f, -0.5f},
           LineVertex{1.0f, -0.5f},
           LineVertex{1.0f, +0.5f},
           LineVertex{0.0f, +0.5f},
},
        wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Vertex)),
   lineIndices(
      std::vector<uint16_t>{
         0, 1, 2, // Triangle #0 connects points #0, #1 and #2
         2, 3, 0  // Triangle #1 connects points #0, #2 and #3
      },
      wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Index)) {}

glm::mat4 CalculateModel(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale) {
   glm::mat4 model           = glm::translate(glm::mat4(1.0f), glm::vec3(objectPosition, 0.0f));
   float     rotationRadians = glm::radians(objectRotationDegrees);
   model                     = glm::rotate(model, rotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
   model                     = glm::scale(model, glm::vec3(objectScale, objectScale, 1.0f));
   return model;
}

glm::mat4 CalculateView() {
   return glm::translate(glm::mat4(1.0f), glm::vec3(-Camera::position, 0.0f));
}

glm::mat4 CalculateProjection() {
   glm::ivec2 windowSize  = Application::get().windowSize();
   float      aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);

   float orthoWidth = Camera::scale * aspectRatio;
   float left       = -orthoWidth / 2.0f;
   float right      = orthoWidth / 2.0f;
   float bottom     = -Camera::scale / 2.0f;
   float top        = Camera::scale / 2.0f;
   float near       = -1.0f;
   float far        = 1.0f;

   return glm::ortho(left, right, bottom, top, near, far);
}

glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale) {
   glm::mat4 projection = CalculateProjection();
   glm::mat4 view       = CalculateView();
   glm::mat4 model      = CalculateModel(objectPosition, objectRotationDegrees, objectScale);

   return projection * view * model;
}

void Renderer::DrawLine(Line line, RenderPass& renderPass) {
   auto                             mvp = CalculateMVP({0, 0}, 0, 1);
   LineVertexUniform                vertexUniform(line.start, line.end, 0.1f, mvp);
   LineFragmentUniform              fragmentUniform{line.color};
   UniformBuffer<LineVertexUniform> vertexUniformBuffer(
      {vertexUniform}, wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Uniform));
   UniformBuffer<LineFragmentUniform> fragmentUniformBuffer(
      {fragmentUniform}, wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Uniform));

   BindGroup bindGroup = LineLayout::ToBindGroup(device, std::forward_as_tuple(vertexUniformBuffer, 0),
                                                 std::forward_as_tuple(fragmentUniformBuffer, 0));

   renderPass.Draw(this->line, linePoints, lineIndices, bindGroup, {});
}


std::vector<Line>& GetDebugLines() {
   static std::vector<Line> debugLines; // Initialized within the function
   return debugLines;
}

void Renderer::DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color) {
   Renderer::DebugLine(start, end, glm::vec4(color, 0.3f));
}

void Renderer::DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color) {
   GetDebugLines().push_back({start, end, color});
}


void Renderer::DrawDebug(RenderPass& renderPass) {
   for (const auto& line : GetDebugLines()) {
      this->DrawLine(line, renderPass);
   }
   GetDebugLines().clear();
}


glm::vec2 Renderer::MousePos() {
   double x, y;

   auto window = Application::get().getWindow();
   glfwGetCursorPos(window, &x, &y);

#ifndef __EMSCRIPTEN__
   float xscale, yscale;
   glfwGetWindowContentScale(window, &xscale, &yscale);
   x *= xscale;
   y *= yscale;
#endif

   return ScreenToWorldPosition({static_cast<float>(x), static_cast<float>(y)});
}

glm::vec2 Renderer::ScreenToWorldPosition(const glm::vec2& screenPos) {
   // Retrieve window size from the renderer
   auto windowSize = Application::get().windowSize();
   auto width      = windowSize.x;
   auto height     = windowSize.y;

   // Calculate aspect ratio
   float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

   // Calculate orthographic projection matrix
   float     orthoWidth = Camera::scale * aspectRatio;
   glm::mat4 projection =
      glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f, -Camera::scale / 2.0f, Camera::scale / 2.0f, -1.0f, 1.0f);

   // Create view matrix
   glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-Camera::position, 0.0f));

   // Inverse of MVP
   glm::mat4 invVP = glm::inverse(projection * view);

   // Normalize screen coordinates (convert to [-1, 1] range)
   glm::vec2 normalizedScreenPos;
   normalizedScreenPos.x = (2.0f * screenPos.x) / width - 1.0f;
   normalizedScreenPos.y = 1.0f - (2.0f * screenPos.y) / height;

   // Convert to world space by applying the inverse matrix
   glm::vec4 clipSpacePos  = glm::vec4(normalizedScreenPos, 0.0f, 1.0f);
   glm::vec4 worldSpacePos = invVP * clipSpacePos;

   // Return world position with z=0
   return glm::vec2(worldSpacePos.x, worldSpacePos.y);
}
