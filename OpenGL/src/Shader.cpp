#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include "Renderer.h"
#include <glm/glm.hpp>

namespace fs = std::filesystem;

Shader::Shader(const std::string& filepath)
   : m_FilePath(filepath)
   , m_RendererID(0) {
   std::cout << "Initializing shader: " << filepath << std::endl;
   ShaderProgramSource source = ParseShader(filepath);
   m_RendererID               = CreateShader(source.VertexSource, source.FragmentSource);
   m_last_write               = fs::last_write_time(fs::path{m_FilePath});
}

Shader::~Shader() {
   if (m_RendererID != 0) {
      GLCall(glDeleteProgram(m_RendererID));
   }
}



ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
   namespace fs = std::filesystem; // Alias for convenience

   std::ifstream stream(filepath);
   if (!stream.is_open()) {
      std::cerr << "Failed to open shader file: " << filepath << std::endl;
      return {};
   }

   // Extract the directory of the shader file for resolving relative includes
   fs::path shaderPath(filepath);
   fs::path shaderDirectory = shaderPath.parent_path();

   enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
   std::string       line;
   std::stringstream ss[2];
   ShaderType        type = ShaderType::NONE;

   // Helper lambda to handle #include directives
   auto handleInclude = [&](const std::string& includePath) -> std::string {
      fs::path      fullIncludePath = shaderDirectory / includePath; // Resolve relative to shader directory
      std::ifstream includeFile(fullIncludePath);
      if (!includeFile.is_open()) {
         std::cerr << "Failed to open include file: " << fullIncludePath << std::endl;
         return "";
      }
      std::stringstream buffer;
      buffer << includeFile.rdbuf();
      return buffer.str();
   };

   while (std::getline(stream, line)) {
      // Trim leading whitespace for accurate directive detection
      std::string trimmedLine = line;
      trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));

      if (trimmedLine.find("#shader") != std::string::npos) {
         if (trimmedLine.find("vertex") != std::string::npos) {
            type = ShaderType::VERTEX;
         } else if (trimmedLine.find("fragment") != std::string::npos) {
            type = ShaderType::FRAGMENT;
         }
      } else if (trimmedLine.find("#include") == 0) { // Check if line starts with #include
         size_t firstQuote = trimmedLine.find('\"');
         size_t lastQuote  = trimmedLine.find_last_of('\"');
         if (firstQuote != std::string::npos && lastQuote != std::string::npos && lastQuote > firstQuote) {
            std::string includePathStr = trimmedLine.substr(firstQuote + 1, lastQuote - firstQuote - 1);
            // Handle the include path relative to shader directory
            std::string includedSource = handleInclude(includePathStr);
            if (!includedSource.empty()) {
               ss[(int)type] << includedSource << '\n';
            }
         } else {
            std::cerr << "Invalid #include directive: " << line << std::endl;
         }
      } else {
         if (type != ShaderType::NONE) { // Ensure shader type is set before appending
            ss[(int)type] << line << '\n';
         }
      }
   }

   return {ss[0].str(), ss[1].str()};
}


uint32_t Shader::CompileShader(uint32_t type, const std::string& source) {
   uint32_t    id  = glCreateShader(type);
   const char* src = source.c_str();
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

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
   if (vertexShader.empty() || fragmentShader.empty())
      return 0;

   uint32_t program = 0;
   uint32_t vs      = CompileShader(GL_VERTEX_SHADER, vertexShader);
   uint32_t fs      = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

   if (vs && fs) {
      program = glCreateProgram();
      glAttachShader(program, vs);
      glAttachShader(program, fs);
      glLinkProgram(program);
      glValidateProgram(program);

      glDeleteShader(vs);
      glDeleteShader(fs);
   }

   return program;
}

void Shader::UpdateIfNeeded() {
   fs::path p{m_FilePath};
   auto     last_write = fs::last_write_time(p);
   if (last_write > m_last_write) {
      ShaderProgramSource source = ParseShader(m_FilePath);
      if (auto id = CreateShader(source.VertexSource, source.FragmentSource)) {
         m_RendererID = id;
         m_last_write = last_write;
      }
   }
}

void Shader::Bind() const {
   GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const {
   GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value) {
   GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value) {
   GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, const glm::vec2& value) {
   GLCall(glUniform2f(GetUniformLocation(name), value.x, value.y));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value) {
   GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& value) {
   GLCall(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
   GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

uint32_t Shader::GetUniformLocation(const std::string& name) {
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
