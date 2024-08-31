#include "Utils.h"
#include <iostream>

void GLClearError() {
   while (glGetError() != GL_NO_ERROR)
      ;
}

bool GLLogCall(const char* function, const char* file, int /* line */) {
   bool ok = true;
   while (GLenum error = glGetError()) {
      std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << " " << std::endl;
      ok = false;
   }
   return ok;
}
