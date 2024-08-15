#include "Renderer.h"

#include <iostream>

void GLClearError() {
   while (glGetError() != GL_NO_ERROR)
      ;
}

bool GLLogCall(const char* function, const char* file, int line) {
   bool ok = true;
   while (GLenum error = glGetError()) {
      std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << " " << std::endl;
      ok = false;
   }
   return ok;
}

void Renderer::Clear() const {
   GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
   shader.Bind();
   va.Bind();
   ib.Bind();
   GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

std::tuple<float, float> Renderer::WindowSize() const {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   return std::tuple<float, float>(width, height);
}
