#include "SquareObject.h"
#include "../Input.h"
#include "../rendering/Texture.h"

SquareObject::SquareObject(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y,
                           std::string texturePath)
   : GameObject(name, drawPriority,
                {
                   tile_x, tile_y
})
   , tile_x(tile_x)
   , tile_y(tile_y)
   , pointBuffer(Buffer<SquareObjectVertex>::create(
        {
           SquareObjectVertex{glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f)}, // 0
           SquareObjectVertex{glm::vec2(0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},  // 1
           SquareObjectVertex{glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 1.0f)},   // 2
           SquareObjectVertex{glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 1.0f)},  // 3
        },
        wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex)),
   indexBuffer(IndexBuffer::create(
      {
         0, 1, 2, // Triangle #0 connects points #0, #1 and #2
         0, 2, 3  // Triangle #1 connects points #0, #2 and #3
      },
      wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index)),
   vertexUniform(BufferView<SquareObjectVertexUniform>::create(
      SquareObjectVertexUniform{CalculateMVP(glm::vec2{tile_x, tile_y}, 0, 1)})),
   fragmentUniform(
      BufferView<SquareObjectFragmentUniform>::create(SquareObjectFragmentUniform{glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)})),
   texture(Texture::create(texturePath)) {}

void SquareObject::render(Renderer& renderer) {
   this->vertexUniform.Update(SquareObjectVertexUniform{CalculateMVP(glm::vec2{tile_x, tile_y}, 0, 1)});
   this->fragmentUniform.Update(SquareObjectFragmentUniform{tintColor});

   renderer.setPipeline(renderer.squareObject);

   wgpu::BindGroup bindGroup =
      SquareObjectLayout::BindGroup(renderer.device, vertexUniform, fragmentUniform, texture.get(), renderer.sampler);
   std::vector<uint32_t> offset{
      (uint32_t)vertexUniform.getOffset(),
      (uint32_t)fragmentUniform.getOffset(),
   };
   renderer.renderPass.setBindGroup(0, bindGroup, offset.size(), offset.data());
   renderer.renderPass.setVertexBuffer(0, pointBuffer->get(), 0, pointBuffer->sizeBytes());
   renderer.renderPass.setIndexBuffer(indexBuffer->get(), wgpu::IndexFormat::Uint16, 0, indexBuffer->sizeBytes());
   renderer.renderPass.drawIndexed(indexBuffer->count(), 1, 0, 0, 0);
}

void SquareObject::update() {
   position    = zeno(position, glm::vec2(tile_x, tile_y), 0.05);
   tintColor.a = zeno(tintColor.a, 0.0, 0.3);
}
