#pragma once

#include <string>
#include <iostream>
#include <webgpu/webgpu.hpp>
#include <filesystem>

class Shader {
public:
   Shader(wgpu::Device device, const std::string& src);
   ~Shader();

   // Deleted copy constructor and assignment operator
   Shader(const Shader&)            = delete;
   Shader& operator=(const Shader&) = delete;

   // Move constructor and assignment operator
   Shader(Shader&& other) noexcept;
   Shader& operator=(Shader&& other) noexcept;

   wgpu::ShaderModule GetShaderModule() const { return shaderModule; }

private:
   wgpu::ShaderModule shaderModule;
};
