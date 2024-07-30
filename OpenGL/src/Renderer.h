#pragma once

#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak(); // Remove this line if you are not using visual studio
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
