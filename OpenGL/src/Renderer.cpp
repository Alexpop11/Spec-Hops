#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "game_objects/Camera.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>

std::string Renderer::res_path;

ImFont* load_font(ImGuiIO* io, const std::string& font_name, int size) {
   auto    f    = Renderer::ResPath() + font_name;
   ImFont* font = io->Fonts->AddFontFromFileTTF(f.c_str(), size);
   return font;
}

glm::vec2 Renderer::ScreenToWorldPosition(const glm::vec2& screenPos) {
   // Retrieve window size from the renderer
   auto [width, height] = WindowSize();

   // Calculate aspect ratio
   float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

   // Calculate orthographic projection matrix
   float     orthoWidth = Camera::scale * aspectRatio;
   glm::mat4 projection =
      glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f, -Camera::scale / 2.0f, Camera::scale / 2.0f, -1.0f, 1.0f);

   // Create view matrix
   glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-Camera::position, 0.0f));

   // Inverse of MVP
   glm::mat4 invVP = glm::inverse(projection * view);

   // Normalize screen coordinates (convert to [-1, 1] range)
   glm::vec2 normalizedScreenPos;
   normalizedScreenPos.x = (2.0f * screenPos.x) / width - 1.0f;
   normalizedScreenPos.y = 1.0f - (2.0f * screenPos.y) / height;

   // Convert to world space by applying the inverse matrix
   glm::vec4 clipSpacePos  = glm::vec4(normalizedScreenPos, 0.0f, 1.0f);
   glm::vec4 worldSpacePos = invVP * clipSpacePos;

   // Return world position with z=0
   return glm::vec2(worldSpacePos.x, worldSpacePos.y);
}

glm::vec2 Renderer::MousePos() {
   double x, y;
   float  xscale, yscale;

   glfwGetCursorPos(window, &x, &y);
   glfwGetWindowContentScale(window, &xscale, &yscale);

   x *= xscale;
   y *= yscale;


   return ScreenToWorldPosition({static_cast<float>(x), static_cast<float>(y)});
}

Renderer::Renderer(GLFWwindow* window, ImGuiIO* io)
   : window(window)
   , io(io)
   , lineShader(Shader(Renderer::ResPath() + "shaders/laser.shader")) {
   std::array<float, 8> positions = {
      0.0f, -0.5f, // Bottom-left
      1.0f, -0.5f, // Bottom-right
      1.0f, 0.5f,  // Top-right
      0.0f, 0.5f,  // Top-left
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

   Renderer::jacquard12_big   = load_font(io, "fonts/Jacquard12.ttf", 40);
   Renderer::jacquard12_small = load_font(io, "fonts/Jacquard12.ttf", 18);
   Renderer::Pixelify         = load_font(io, "fonts/PixelifySans.ttf", 18);
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

void Renderer::DrawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color) {
   lineShader.Bind();
   lineShader.SetUniform4f("u_Color", color);
   lineShader.SetUniform2f("u_StartPos", start);
   lineShader.SetUniform2f("u_EndPos", end);
   lineShader.SetUniform1f("u_Width", 0.1f);

   auto mvp = CalculateMVP(WindowSize(), {0, 0}, 0, 1);
   // Pass MVP matrix to the shader
   lineShader.SetUniformMat4f("u_MVP", mvp);

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

std::vector<Line>& Renderer::GetDebugLines() {
   static std::vector<Line> debugLines; // Initialized within the function
   return debugLines;
}

void Renderer::DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color) {
   Renderer::DebugLine(start, end, glm::vec4(color, 0.3f));
}

void Renderer::DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color) {
   GetDebugLines().push_back({start, end, color});
}


void Renderer::DrawDebug() {
   for (const auto& line : GetDebugLines()) {
      this->DrawLine(line.start, line.end, line.color); // Use the Line function to draw
   }
   GetDebugLines().clear(); // Clear the vector after drawing
}

ImFont* Renderer::jacquard12_big   = nullptr;
ImFont* Renderer::jacquard12_small = nullptr;
ImFont* Renderer::Pixelify         = nullptr;
