
#include <webgpu/webgpu.hpp>

#include <GL/glew.h> // TODO: remove this, we don't need glew since we're using webgpu
#include <GLFW/glfw3.h>



class Application {
public:
   // Initialize everything and return true if it went all right
   Application();

   void InitializeBuffers();

   void InitializePipeline();

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

   void                 PrintAdapterInfo(wgpu::Adapter& adapter);
   wgpu::TextureView    GetNextSurfaceTextureView();
   wgpu::RequiredLimits GetRequiredLimits(wgpu::Adapter& adapter) const;

   GLFWwindow*                          window;
   wgpu::Device                         device;
   wgpu::Queue                          queue;
   wgpu::Surface                        surface;
   std::unique_ptr<wgpu::ErrorCallback> uncapturedErrorCallbackHandle;
   wgpu::TextureFormat                  surfaceFormat = wgpu::TextureFormat::Undefined;
   wgpu::RenderPipeline                 pipeline;

   wgpu::Buffer pointBuffer;
   uint32_t     pointCount;
   wgpu::Buffer indexBuffer;
   uint32_t     indexCount;
};
