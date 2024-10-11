#include "SquareObject.h"
#include "../Input.h"

SquareObject::SquareObject(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y,
                           std::string texturePath)
   : GameObject(name, drawPriority,
                {
                   tile_x, tile_y
})
   , tile_x(tile_x)
   , tile_y(tile_y)
   , pointBuffer(Buffer<SquareObjectVertex>(
        {
           SquareObjectVertex{glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f)}, // 0
           SquareObjectVertex{glm::vec2(0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},  // 1
           SquareObjectVertex{glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 1.0f)},   // 2
           SquareObjectVertex{glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 1.0f)},  // 3
        },
        wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex)),
   indexBuffer(IndexBuffer(
      {
         0, 1, 2, // Triangle #0 connects points #0, #1 and #2
         0, 2, 3  // Triangle #1 connects points #0, #2 and #3
      },
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index)),
   uniformBuffer(
      UniformBuffer<SquareObjectFragmentUniform>({}, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform)) {}

void SquareObject::render(Renderer& renderer) {
   // TODO: Implement
}

void SquareObject::update() {
   position    = zeno(position, glm::vec2(tile_x, tile_y), 0.05);
   tintColor.a = zeno(tintColor.a, 0.0, 0.3);
}
