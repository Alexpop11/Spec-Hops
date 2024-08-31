#include "Background.h"

Background::Background(const std::string& name, int drawPriority, float x, float y)
   : GameObject(name, drawPriority, x, y) {
   shader       = std::make_shared<Shader>(Renderer::ResPath() + "shaders/stars.shader");
   drawPriority = 0;

   float positions[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
   };

   unsigned int indices[] = {0, 1, 2, 2, 3, 0};

   unsigned int vao;
   GLCall(glGenVertexArrays(1, &vao));
   GLCall(glBindVertexArray(vao));

   va = std::make_optional<VertexArray>();
   vb = std::make_optional(VertexBuffer(positions, 4 * 2 * sizeof(float)));
   VertexBufferLayout layout;
   layout.Push<float>(2);
   va->AddBuffer(*vb, layout);

   ib = std::make_optional(IndexBuffer(indices, 6));
}

void Background::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
}

void Background::render(Renderer& renderer) {

   GameObject::render(renderer);

   // draw if va, ib, and shader are set:
   if (va && ib && shader) {
      renderer.Draw(*va, *ib, *shader);
   }
}

void Background::update() {
   // Update logic for Background
}
