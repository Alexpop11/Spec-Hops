#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
   : m_FilePath(filepath)
   , m_RendererID(0) {
   ShaderProgramSource source = ParseShader(filepath);
   m_RendererID               = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader() {
   if (m_RendererID != 0) {
      GLCall(glDeleteProgram(m_RendererID));
   }
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
   std::ifstream stream(filepath);

   enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

   std::string       line;
   std::stringstream ss[2];
   ShaderType        type = ShaderType::NONE;
   while (getline(stream, line)) {
      if (line.find("#shader") != std::string::npos) {
         if (line.find("vertex") != std::string::npos) {
            type = ShaderType::VERTEX;
         } else if (line.find("fragment") != std::string::npos) {
            type = ShaderType::FRAGMENT;
         }
      } else {
         ss[(int)type] << line << '\n';
      }
   }

   return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
   unsigned int id  = glCreateShader(type);
   const char*  src = source.c_str();
   glShaderSource(id, 1, &src, nullptr);
   glCompileShader(id);

   int result;
   glGetShaderiv(id, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE) {
      int length;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glGetShaderInfoLog(id, length, &length, message);
      std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << std::endl;
      std::cout << message << std::endl;
      glDeleteShader(id);
      return 0;
   }

   return (id);
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
   unsigned int program = glCreateProgram();
   unsigned int vs      = CompileShader(GL_VERTEX_SHADER, vertexShader);
   unsigned int fs      = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

   glAttachShader(program, vs);
   glAttachShader(program, fs);
   glLinkProgram(program);
   glValidateProgram(program);

   glDeleteShader(vs);
   glDeleteShader(fs);

   return program;
}

Shader::Shader(Shader&& other) noexcept
   : m_FilePath(std::move(other.m_FilePath))
   , m_RendererID(other.m_RendererID)
   , m_UniformLocationCache(std::move(other.m_UniformLocationCache)) {
   // Transfer ownership of the shader program
   other.m_RendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
   if (this != &other) {
      // Delete current resources
      GLCall(glDeleteProgram(m_RendererID));

      // Transfer ownership
      m_FilePath             = std::move(other.m_FilePath);
      m_RendererID           = other.m_RendererID;
      m_UniformLocationCache = std::move(other.m_UniformLocationCache);

      // Reset other's m_RendererID
      other.m_RendererID = 0;
   }
   return *this;
}

void Shader::Bind() const {
   GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const {
   GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
   GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1f(const std::string& name, float v0) {
   GLCall(glUniform1f(GetUniformLocation(name), v0));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1) {
   GLCall(glUniform2f(GetUniformLocation(name), v0, v1));
}

unsigned int Shader::GetUniformLocation(const std::string& name) {
   if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
      return m_UniformLocationCache[name];

   GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
   if (location == -1 && name.find("u_StartTime") == std::string::npos && name.find("u_Color") == std::string::npos &&
       name.find("u_Time") == std::string::npos && name.find("u_Resolution") == std::string::npos) {

      std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
   }
   m_UniformLocationCache[name] = location;
   return location;
}