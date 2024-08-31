#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "res_path.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Utils.h"


class Renderer {
public:
   GLFWwindow*          window;
   void                 Clear() const;
   void                 Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
   std::tuple<int, int> WindowSize() const;

   static const std::string& ResPath();

private:
   static std::string res_path;
};
