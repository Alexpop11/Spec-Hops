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

#include "imgui.h"

class Line {
public:
   glm::vec2 start;
   glm::vec2 end;
   glm::vec4 color;
};

class Renderer {
public:
   Renderer(GLFWwindow* window, ImGuiIO* io);
   ~Renderer();

   Renderer(const Renderer&)             = delete;
   Renderer(Renderer&& other)            = default;
   Renderer& operator=(const Renderer&)  = delete;
   Renderer& operator=(Renderer&& other) = default;
   glm::vec2 ScreenToWorldPosition(const glm::vec2& screenPos);
   glm::vec2 MousePos();


   void                 Clear() const;
   void                 Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
   void                 DrawDebug();
   std::tuple<int, int> WindowSize() const;

   static const std::string& ResPath();
   static void               DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
   static void               DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);

   // Window pointer
   GLFWwindow* window;

   // IMGUI IO
   ImGuiIO* io;

   // Shader for rendering lines
   Shader                        lineShader;
   std::shared_ptr<VertexBuffer> lineVb;
   std::shared_ptr<IndexBuffer>  lineIb;
   std::shared_ptr<VertexArray>  lineVa;

   static ImFont* jacquard12_big;
   static ImFont* jacquard12_small;
   static ImFont* Pixelify;


private:
   static std::string        res_path;
   void                      DrawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);
   static std::vector<Line>& GetDebugLines();
};

glm::mat4 CalculateMVP(std::tuple<int, int> windowSize, const glm::vec2& objectPosition, float objectRotationDegrees,
                       float objectScale);
