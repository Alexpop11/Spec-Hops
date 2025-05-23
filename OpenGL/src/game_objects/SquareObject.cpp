#include "SquareObject.h"
#include "../Input.h"
#include "../rendering/Texture.h"

SquareObject::SquareObject(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y,
                           std::string texturePath)
   : GameObject(name, drawPriority,
                {
                   tile_x, tile_y
})
   , tilePosition({tile_x, tile_y})
   , pointBuffer(Buffer<SquareObjectVertex>::create(
        {
           SquareObjectVertex{glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f)}, // 0
           SquareObjectVertex{glm::vec2(0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},  // 1
           SquareObjectVertex{glm::vec2(0.5f, 0.5f), glm::vec2(1.0f, 1.0f)},   // 2
           SquareObjectVertex{glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 1.0f)},  // 3
        },
        wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Vertex))),
   indexBuffer(IndexBuffer::create(
      {
         0, 1, 2, // Triangle #0 connects points #0, #1 and #2
         0, 2, 3  // Triangle #1 connects points #0, #2 and #3
      },
      wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Index))),
   vertexUniform(UniformBufferView<SquareObjectVertexUniform>::create(SquareObjectVertexUniform{MVP()})),
   fragmentUniform(
      UniformBufferView<SquareObjectFragmentUniform>::create(SquareObjectFragmentUniform(tintColor, opacity))),
   texture(Texture::create(texturePath)) {}

void SquareObject::render(Renderer& renderer, RenderPass& renderPass) {
   this->vertexUniform.Update(SquareObjectVertexUniform{MVP()});
   this->fragmentUniform.Update(SquareObjectFragmentUniform(tintColor, opacity));
   BindGroup bindGroup =
      SquareObjectLayout::ToBindGroup(renderer.device, vertexUniform, fragmentUniform, texture.get(), renderer.sampler);
   renderPass.Draw(renderer.squareObject, *pointBuffer, *indexBuffer, bindGroup,
                   {
                      (uint32_t)vertexUniform.getOffset(),
                      (uint32_t)fragmentUniform.getOffset(),
                   });
}

void SquareObject::update() {
   position    = zeno(position, getTile(), 0.05);
   tintColor.a = zeno(tintColor.a, 0.0, 0.3);
}
