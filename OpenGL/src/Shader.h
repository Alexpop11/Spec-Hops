#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

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
   // caching for uniforms

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
   void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3);
   void SetUniform1f(const std::string& name, float v0);
   void SetUniform2f(const std::string& name, float v0, float v1);

private:
   ShaderProgramSource ParseShader(const std::string& filepath);
   unsigned int        CompileShader(unsigned int type, const std::string& source);
   unsigned int        CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
   unsigned int        GetUniformLocation(const std::string& name);
};