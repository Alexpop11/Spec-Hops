#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include "WeakMemoizeConstructor.hpp"
#include <glm/glm.hpp>
#include "Utils.h"

struct ShaderProgramSource {
   std::string VertexSource;
   std::string FragmentSource;
};

class Shader {
private:
   std::string                          m_FilePath;
   std::filesystem::file_time_type      m_last_write;
   wrap_t<uint32_t>                     m_RendererID;
   std::unordered_map<std::string, int> m_UniformLocationCache;

public:
   Shader(const std::string& filepath);
   ~Shader();

   Shader(const Shader&)             = delete;
   Shader(Shader&& other)            = default;
   Shader& operator=(const Shader&)  = delete;
   Shader& operator=(Shader&& other) = default;

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
   uint32_t            CompileShader(uint32_t type, const std::string& source);
   uint32_t            CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
   uint32_t            GetUniformLocation(const std::string& name);
};
