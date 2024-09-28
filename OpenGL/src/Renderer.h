#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <tuple>

#include "res_path.hpp"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Utils.h"
#include "AudioEngine.h"

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
   std::tuple<int, int> WindowSize() const;

   static const std::string& ResPath();

   // Window pointer
   GLFWwindow* window;

private:
   static std::string res_path;
};
