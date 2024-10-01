#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <tuple>
#include <vector> // Include vector header

#include "res_path.hpp"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Utils.h"
#include "AudioEngine.h"
#include "Shader.h"

class Line {
public:
   glm::vec2 start;
   glm::vec2 end;
   glm::vec4 color;
};
class Renderer {
public:
   Renderer(GLFWwindow* window);
   ~Renderer();

   Renderer(const Renderer&)             = delete;
   Renderer(Renderer&& other)            = default;
   Renderer& operator=(const Renderer&)  = delete;
   Renderer& operator=(Renderer&& other) = default;

   void                 Clear() const;
   void                 Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
   void                 DrawDebug();
   std::tuple<int, int> WindowSize() const;

   static const std::string& ResPath();
   static void               DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
   static void               DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);

   // Window pointer
   GLFWwindow* window;

   // Shader for rendering lines
   Shader                        lineShader;
   std::shared_ptr<VertexBuffer> lineVb;
   std::shared_ptr<IndexBuffer>  lineIb;
   std::shared_ptr<VertexArray>  lineVa;


private:
   static std::string        res_path;
   void                      DrawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);
   static std::vector<Line>& GetDebugLines();
};
