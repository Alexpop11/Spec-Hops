#include "SquareObject.h"

SquareObject::SquareObject(const std::string& name, int drawPriority, int tile_x, int tile_y, std::string texturePath)
   : GameObject(name, drawPriority, {tile_x, tile_y})
   , tile_x(tile_x)
   , tile_y(tile_y) {
   texture = Texture::create(Renderer::ResPath() + texturePath);
   shader  = Shader::create(Renderer::ResPath() + "shaders/shader.shader");

   std::array<float, 16> positions = {
      -0.5f, -0.5f, 0.0f, 0.0f, // 0
      0.5f,  -0.5f, 1.0f, 0.0f, // 1
      0.5f,  0.5f,  1.0f, 1.0f, // 2
      -0.5f, 0.5f,  0.0f, 1.0f  // 3
   };

   std::array<unsigned int, 6> indices = {0, 1, 2, 2, 3, 0};

   unsigned int vao;
   GLCall(glGenVertexArrays(1, &vao));
   GLCall(glBindVertexArray(vao));

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
}
