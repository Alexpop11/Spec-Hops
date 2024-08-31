#include "Renderer.h"

#include <iostream>

std::string Renderer::res_path;

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
      // Get the path of the executable
      fs::path exe_path = fs::weakly_canonical(fs::path("/proc/self/exe"));
      fs::path exe_dir  = exe_path.parent_path();

      // Check for "res/shaders" relative to the executable's directory
      if (fs::exists(exe_dir / "res" / "shaders"))
         res_path = (exe_dir / "res").string() + "/";
      else if (fs::exists(RES_PATH "/res/shaders"))
         res_path = RES_PATH "/res/";
      else
         std::cout << "Resource directory not found\n";
   }

   return res_path;
}

std::tuple<int, int> Renderer::WindowSize() const {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   return {width, height};
}
