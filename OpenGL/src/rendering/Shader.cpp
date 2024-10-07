// Shader.cpp
#include "Shader.h"

Shader::Shader(wgpu::Device device, const std::string& src) {
   wgpu::ShaderModuleWGSLDescriptor wgslDesc;
   wgslDesc.code       = src.c_str();
   wgslDesc.chain.next = nullptr;
   // I changed this from ShaderModuleWGSLDescriptor to ShaderSourceWGSL
   wgslDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;



   wgpu::ShaderModuleDescriptor moduleDesc;
#ifdef WEBGPU_BACKEND_WGPU
   shaderDesc.hintCount = 0;
   shaderDesc.hints     = nullptr;
#endif

   moduleDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&wgslDesc);
   moduleDesc.label       = "Shader Module";

   shaderModule = device.createShaderModule(moduleDesc);

   if (!shaderModule) {
      std::cerr << "Failed to create shader module." << std::endl;
   }
}

Shader::~Shader() {
   if (shaderModule) {
      shaderModule.release();
   }
}

Shader::Shader(Shader&& other) noexcept
   : shaderModule(other.shaderModule) {
   other.shaderModule = nullptr;
}

Shader& Shader::operator=(Shader&& other) noexcept {
   if (this != &other) {
      if (shaderModule) {
         shaderModule.release();
      }
      shaderModule       = other.shaderModule;
      other.shaderModule = nullptr;
   }
   return *this;
}
