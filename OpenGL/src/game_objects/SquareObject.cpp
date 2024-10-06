#include "SquareObject.h"

SquareObject::SquareObject(const std::string& name, DrawPriority drawPriority, int tile_x, int tile_y,
                           std::string texturePath)
   : GameObject(name, drawPriority, {tile_x, tile_y})
   , tile_x(tile_x)
   , tile_y(tile_y) {
   texture = Texture::create(Renderer::ResPath() + texturePath);
   shader  = Shader::create(Renderer::ResPath() + "shaders/shader.shader");

   std::array<glm::vec2, 8> positions = {
      glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f), // 0
      glm::vec2(0.5f, -0.5f),  glm::vec2(1.0f, 0.0f), // 1
      glm::vec2(0.5f, 0.5f),   glm::vec2(1.0f, 1.0f), // 2
      glm::vec2(-0.5f, 0.5f),  glm::vec2(0.0f, 1.0f)  // 3
   };

   std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

   vb = VertexBuffer::create(positions);
   VertexBufferLayout layout;
   layout.Push<float>(2);
   layout.Push<float>(2);
   va = std::make_shared<VertexArray>(vb, layout);
   ib = IndexBuffer::create(indices);
}

void SquareObject::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
   texture->Bind();
   shader->SetUniform1i("u_Texture", 0);
   shader->SetUniform4f("u_Color", tintColor);
}

void SquareObject::render(Renderer& renderer) {
   GameObject::render(renderer);

   // draw if va, ib, and shader are set:
   if (va && ib && shader) {
      renderer.Draw(*va, *ib, *shader);
   }
}

void SquareObject::update() {
   position = zeno(position, glm::vec2(tile_x, tile_y), 0.05);
   tintColor.a = zeno(tintColor.a, 0.0, 0.3);
}
