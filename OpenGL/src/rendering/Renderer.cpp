#include "Renderer.h"
#include "Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "../game_objects/Camera.h"

Renderer::Renderer()
   : stars(RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>>(
        "stars.wgsl"))
   , squareObject(RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>(
        "square_object.wgsl"))
   , line(RenderPipeline<BindGroupLayouts<LineLayout>, VertexBufferLayout<LineVertex>>("line.wgsl"))
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

void Renderer::DrawLine(Line line) {
   auto                               mvp = CalculateMVP({0, 0}, 0, 1);
   LineVertexUniform                  vertexUniform(line.start, line.end, 0.1f, mvp);
   LineFragmentUniform                fragmentUniform{line.color};
   UniformBuffer<LineVertexUniform>   vertexUniformBuffer({vertexUniform},
                                                          wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
   UniformBuffer<LineFragmentUniform> fragmentUniformBuffer({fragmentUniform},
                                                            wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
   auto                               points = std::vector<LineVertex>{
      LineVertex{-1, -1},
      LineVertex{+1, -1},
      LineVertex{+1, +1},
      LineVertex{-1, +1},
   };
   Buffer<LineVertex> pointBuffer(points, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex);
   auto               indices = std::vector<uint16_t>{
      0, 1, 2, // Triangle #0 connects points #0, #1 and #2
      0, 2, 3  // Triangle #1 connects points #0, #2 and #3
   };
   IndexBuffer indexBuffer(indices, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index);

   BindGroup bindGroup = this->line
                                  .BindGroups(std::forward_as_tuple(std::forward_as_tuple(vertexUniformBuffer, 0),
                                                                    std::forward_as_tuple(fragmentUniformBuffer, 0)))
                                  .front();

   this->setPipeline(this->line);
   this->renderPass.setBindGroup(0, bindGroup.get(), 0, nullptr);
   this->renderPass.setVertexBuffer(0, pointBuffer.get(), 0, pointBuffer.sizeBytes());
   this->renderPass.setIndexBuffer(indexBuffer.get(), wgpu::IndexFormat::Uint16, 0, indexBuffer.sizeBytes());
   this->renderPass.drawIndexed(indexBuffer.count(), 1, 0, 0, 0);
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


void Renderer::DrawDebug() {
   for (const auto& line : GetDebugLines()) {
      this->DrawLine(line);
   }
   GetDebugLines().clear();
}
