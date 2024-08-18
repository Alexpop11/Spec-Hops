#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "res_path.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"


#ifdef _MSC_VER
   #define ASSERT(x) \
      if (!(x))      \
         __debugbreak();
#else
   #include <cassert>
   #define ASSERT assert
#endif

#define GLCall(x)  \
   GLClearError(); \
   x;              \
   ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
   GLFWwindow*              window;
   void                     Clear() const;
   void                     Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
   std::tuple<float, float> WindowSize() const;

   static const std::string& ResPath();

private:
   static std::string res_path;
};
