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

template <class T>
class wrap_t {
private:
   T v;

public:
   wrap_t(T v = 0) : v(v) {}

   wrap_t(const wrap_t&)            = default;
   wrap_t& operator=(const wrap_t&) = default;

   wrap_t(wrap_t&& o)             { v = o.v; o.v = 0; }
   wrap_t& operator=(wrap_t&& o)  { v = o.v; o.v = 0; return *this; }

   operator T&()              { return v; }
   T* operator&()             { return &v; }
   operator const T&() const  { return v; }
   const T* operator&() const { return &v; }

};