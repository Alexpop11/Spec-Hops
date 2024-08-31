#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

