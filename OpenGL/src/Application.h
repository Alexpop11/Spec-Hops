
#include <webgpu/webgpu.hpp>

#include "rendering/RenderPipeline.h"
#include "rendering/Buffer.h"
#include "rendering/BindGroupLayout.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Laid out with
// https://eliemichel.github.io/WebGPU-AutoLayout/
struct StarUniforms {
   float     time; // at byte offset 0
   float     _pad0;
   glm::vec2 resolution; // at byte offset 8
};

using StarUniformBinding =
   BufferBinding<StarUniforms,                                                       // Type of the buffer
                 wgpu::both(wgpu::ShaderStage::Vertex, wgpu::ShaderStage::Fragment), // Shader visibility
                 wgpu::BufferBindingType::Uniform,                                   // Buffer binding type
                 true>;

class Application {
public:
   // Initialize everything and return true if it went all right
   Application();

   // Uninitialize everything that was initialized
   void Terminate();

   // Draw a frame and handle events
   void MainLoop();

   // Return true as long as the main loop should keep on running
   bool IsRunning();

   static Application& get();

   bool        initialized;
   std::string res_path;

private:
   void InitializeResPath();

   wgpu::TextureView GetNextSurfaceTextureView();

   GLFWwindow*                                         window;
   wgpu::Instance                                      instance;
   wgpu::Surface                                       surface;
   wgpu::Adapter                                       adapter;
   wgpu::Device                                        device;
   std::unique_ptr<wgpu::ErrorCallback>                uncapturedErrorCallbackHandle;
   wgpu::Queue                                         queue;
   wgpu::TextureFormat                                 surfaceFormat;
   RenderPipeline<BindGroupLayout<StarUniformBinding>> pipeline;

   Buffer<float>               pointBuffer;
   Buffer<uint16_t>            indexBuffer;
   UniformBuffer<StarUniforms> uniformBuffer;
};
