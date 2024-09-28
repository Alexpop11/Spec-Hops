#include "Renderer.h"
#include "VertexBufferLayout.h"

#include <iostream>

std::string Renderer::res_path;

Renderer::Renderer(GLFWwindow* window)
   : window(window)
   , lineShader(Shader(Renderer::ResPath() + "shaders/line.shader")) {
   std::array<float, 8> positions = {
      -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
   };

   std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

   uint32_t vao;
   GLCall(glGenVertexArrays(1, &vao));
   GLCall(glBindVertexArray(vao));

   lineVb = VertexBuffer::create(positions);
   VertexBufferLayout layout;
   layout.Push<float>(2);
   lineVa = std::make_shared<VertexArray>(lineVb, layout);
   lineIb = IndexBuffer::create(indices);
}

Renderer::~Renderer() {}

void Renderer::Clear() const {
   GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
   shader.Bind();
   va.Bind();
   ib.Bind();
   GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Line(glm::vec2 start, glm::vec2 end, glm::vec3 color) {
   lineShader.Bind();
   lineShader.SetUniform4f("u_Color", glm::vec4(color.r, color.g, color.b, 1.0f));
   lineShader.SetUniform2f("u_StartPos", glm::vec2(start.x, start.y));
   lineShader.SetUniform2f("u_EndPos", glm::vec2(end.x, end.y));

   // create a vertex buffer and index buffer
   Draw(*lineVa, *lineIb, lineShader);
}

const std::string& Renderer::ResPath() {
   namespace fs = std::filesystem;

   if (res_path.empty()) {
      // Get the path of the executable
      fs::path exe_path = fs::weakly_canonical(fs::path("/proc/self/exe"));
      fs::path exe_dir  = exe_path.parent_path();

      // Check for "res/shaders" relative to the executable's directory
      if (fs::exists(exe_dir / "res" / "shaders")) {
         res_path = (exe_dir / "res").string() + "/";
      } else if (fs::exists(RES_PATH "/res/shaders")) {
         res_path = RES_PATH "/res/";
      } else {
         std::cout << "Resource directory not found relative to " << exe_dir << " or " << RES_PATH << std::endl;
         res_path = "./res/";
      }
   }

   return res_path;
}

std::tuple<int, int> Renderer::WindowSize() const {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   return {width, height};
}
