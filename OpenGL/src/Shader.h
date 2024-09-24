#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include "WeakMemoizeConstructor.h"
#include <glm/glm.hpp>

struct ShaderProgramSource {
   std::string VertexSource;
   std::string FragmentSource;
};

class Shader {
private:
   std::string                          m_FilePath;
   std::filesystem::file_time_type      m_last_write;
   unsigned int                         m_RendererID;
   std::unordered_map<std::string, int> m_UniformLocationCache;

public:
   Shader(const std::string& filepath);
   Shader(Shader&& other) noexcept;
   Shader(const Shader&)            = delete;
   Shader& operator=(const Shader&) = delete;
   Shader& operator=(Shader&& other) noexcept;
   ~Shader();

   void UpdateIfNeeded();
   void Bind() const;
   void Unbind() const;

   // Set Uniforms
   void SetUniform1i(const std::string& name, int value);
   void SetUniform1f(const std::string& name, float value);
   void SetUniform2f(const std::string& name, const glm::vec2& value);
   void SetUniform3f(const std::string& name, const glm::vec3& value);
   void SetUniform4f(const std::string& name, const glm::vec4& value);
   void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(Shader)

private:
   ShaderProgramSource ParseShader(const std::string& filepath);
   unsigned int        CompileShader(unsigned int type, const std::string& source);
   unsigned int        CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
   unsigned int        GetUniformLocation(const std::string& name);
};
