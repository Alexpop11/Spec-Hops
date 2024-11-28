#include "Shader.h"
#include "../Application.h"

Shader::Shader(wgpu::Device device, const std::filesystem::path& filePath)
   : filePath(filePath) {
   // Read the shader source code from the file
   auto fullPath = Application::get().res_path / "shaders" / filePath;
   std::string src = ReadFile(fullPath);
   if (src.empty()) {
      std::cerr << "Shader source is empty. Failed to load shader from: " << filePath << std::endl;
      return;
   }

   wgpu::ShaderModuleWGSLDescriptor wgslDesc;
   wgslDesc.code       = src.c_str();
   wgslDesc.chain.next = nullptr;
   // I changed this from ShaderModuleWGSLDescriptor to ShaderSourceWGSL
#ifdef __EMSCRIPTEN__
   wgslDesc.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
#else
   wgslDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
#endif

   wgpu::ShaderModuleDescriptor moduleDesc;
#ifdef WEBGPU_BACKEND_WGPU
   shaderDesc.hintCount = 0;
   shaderDesc.hints     = nullptr;
#endif

   std::string label      = filePath.stem().string();
   moduleDesc.nextInChain = reinterpret_cast<WGPUChainedStruct*>(&wgslDesc);
   moduleDesc.label       = label.c_str();

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

std::string Shader::ProcessIncludes(const std::string& source, const std::filesystem::path& shaderPath) const {
    std::istringstream stream(source);
    std::ostringstream result;
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("#include") == 0) {
            size_t firstQuote = line.find('<');
            size_t lastQuote = line.find('>');
            if (firstQuote != std::string::npos && lastQuote != std::string::npos) {
                std::string filename = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                auto includePath = shaderPath.parent_path() / filename;
                
                // Read the included file
                std::string includeContent = ReadFile(includePath);
                if (!includeContent.empty()) {
                    result << includeContent << '\n';
                }
            }
        } else {
            result << line << '\n';
        }
    }
    return result.str();
}

std::string Shader::ReadFile(const std::filesystem::path& path) const {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Shader file does not exist: " << path << std::endl;
        return "";
    }

    std::ifstream fileStream(path, std::ios::in | std::ios::binary);
    if (!fileStream) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::ostringstream contents;
    contents << fileStream.rdbuf();
    fileStream.close();

    // Process includes in the file content
    return ProcessIncludes(contents.str(), path);
}
