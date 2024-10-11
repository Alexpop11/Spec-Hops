#pragma once
#include <webgpu/webgpu.hpp>
#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Application {
public:
   // Initialize everything and return true if it went all right
   Application();

   // Uninitialize everything that was initialized
   void Terminate();

   // Return true as long as the main loop should keep on running
   bool IsRunning();

   static Application& get();

   bool                  initialized;
   std::filesystem::path res_path;

   wgpu::TextureFormat& getSurfaceFormat() { return surfaceFormat; }
   wgpu::Device&        getDevice() { return device; }
   wgpu::Surface&       getSurface() { return surface; }
   wgpu::Queue&         getQueue() { return queue; }

   wgpu::TextureView GetNextSurfaceTextureView();
   void              onResize();
   void              configureSurface();
   glm::ivec2        windowSize();

private:
   void InitializeResPath();


   GLFWwindow*                          window;
   wgpu::Instance                       instance;
   wgpu::Surface                        surface;
   wgpu::Adapter                        adapter;
   wgpu::Device                         device;
   std::unique_ptr<wgpu::ErrorCallback> uncapturedErrorCallbackHandle;
   wgpu::Queue                          queue;
   wgpu::TextureFormat                  surfaceFormat;

   // Buffer<float>               pointBuffer;
   // IndexBuffer            indexBuffer;
   // UniformBuffer<StarUniforms> uniformBuffer;

   // Buffer<SquareObjectVertex>                 squareObjectPointBuffer;
   // UniformBuffer<SquareObjectVertexUniform>   squareObjectVertexUniform;
   // UniformBuffer<SquareObjectFragmentUniform> squareObjectFragmentUniform;
};
