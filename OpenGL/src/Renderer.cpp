#include "Renderer.h"

#include <iostream>

std::string Renderer::res_path;

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

const std::string& Renderer::ResPath() {
   namespace fs = std::filesystem;
   if (res_path.empty()) {
      if (fs::exists("./res/shaders"))
         res_path = "./res/";
      else if (fs::exists(RES_PATH "/res/shaders"))
         res_path = RES_PATH "/res/";
      else
         std::cout << "Resource directory not found\n";
   }
   return res_path;
}

std::tuple<float, float> Renderer::WindowSize() const {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   return { width, height };
}
