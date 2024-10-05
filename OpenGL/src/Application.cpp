#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <webgpu/webgpu.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>

#define STB_IMAGE_IMPLEMENTATION // for icon
#include "stb_image.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "game_objects/GameObject.h"
#include "Input.h"
#include "World.h"
#include "game_objects/Player.h"
#include "game_objects/Background.h"
#include "game_objects/Camera.h"
#include "game_objects/Tile.h"
#include "game_objects/enemies/Bomber.h"
#include "Texture.h"
#include "game_objects/Fog.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <glfw3webgpu.h>
#include "gpu/webgpu-utils.h"

#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
   #include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

void setWindowIcon(GLFWwindow* window, const char* iconPath) {
   int            width, height, channels;
   unsigned char* pixels = stbi_load(iconPath, &width, &height, &channels, 4);
   if (pixels) {
      GLFWimage images[1];
      images[0] = {.width = width, .height = height, .pixels = pixels};
      glfwSetWindowIcon(window, 1, images);
      stbi_image_free(pixels);
   } else {
      std::cerr << "Failed to load icon: " << iconPath << std::endl;
   }
}

void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) {
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      std::cout << "Escape key was pressed" << std::endl;
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
}

void deviceLost(WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
   std::cout << "Device lost: reason " << reason;
   if (message)
      std::cout << " (" << message << ")";
   std::cout << std::endl;
}


class Application {
public:
   // Initialize everything and return true if it went all right
   bool Initialize() {
      // Open window
      glfwInit();
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      window = glfwCreateWindow(640, 480, "Learn WebGPU", nullptr, nullptr);

      WGPUInstance instance = wgpuCreateInstance(nullptr);

      std::cout << "Requesting adapter..." << std::endl;
      surface                               = glfwGetWGPUSurface(instance, window);
      WGPURequestAdapterOptions adapterOpts = {};
      adapterOpts.nextInChain               = nullptr;
      adapterOpts.compatibleSurface         = surface;
      WGPUAdapter adapter                   = requestAdapterSync(instance, &adapterOpts);
      std::cout << "Got adapter: " << adapter << std::endl;

      wgpuInstanceRelease(instance);

      std::cout << "Requesting device..." << std::endl;
      WGPUDeviceDescriptor deviceDesc     = {};
      deviceDesc.nextInChain              = nullptr;
      deviceDesc.label                    = "My Device";
      deviceDesc.requiredFeatureCount     = 0;
      deviceDesc.requiredLimits           = nullptr;
      deviceDesc.defaultQueue.nextInChain = nullptr;
      deviceDesc.defaultQueue.label       = "The default queue";
      deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
         std::cout << "Device lost: reason " << reason;
         if (message)
            std::cout << " (" << message << ")";
         std::cout << std::endl;
      };
      device = requestDeviceSync(adapter, &deviceDesc);
      std::cout << "Got device: " << device << std::endl;

      auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
         std::cout << "Uncaptured device error: type " << type;
         if (message)
            std::cout << " (" << message << ")";
         std::cout << std::endl;
      };
      wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);

      queue = wgpuDeviceGetQueue(device);

      // Configure the surface
      WGPUSurfaceConfiguration config = {};
      config.nextInChain              = nullptr;

      // Configuration of the textures created for the underlying swap chain
      config.width                    = 640;
      config.height                   = 480;
      config.usage                    = WGPUTextureUsage_RenderAttachment;
      WGPUTextureFormat surfaceFormat = wgpuSurfaceGetPreferredFormat(surface, adapter);
      config.format                   = surfaceFormat;

      // And we do not need any particular view format:
      config.viewFormatCount = 0;
      config.viewFormats     = nullptr;
      config.device          = device;
      config.presentMode     = WGPUPresentMode_Fifo;
      config.alphaMode       = WGPUCompositeAlphaMode_Auto;

      wgpuSurfaceConfigure(surface, &config);

      // Release the adapter only after it has been fully utilized
      wgpuAdapterRelease(adapter);

      return true;
   }

   // Uninitialize everything that was initialized
   void Terminate() {
      std::cout << "Terminating application" << std::endl;
      // Release the queue
      wgpuQueueRelease(queue);
      // Release the device
      wgpuDeviceRelease(device);

      // Unconfigure the surface
      wgpuSurfaceUnconfigure(surface);
      // Release the surface
      wgpuSurfaceRelease(surface);


      // Close the window and terminate GLFW
      glfwDestroyWindow(window);
      glfwTerminate();
   }

   WGPUTextureView GetNextSurfaceTextureView() {
      WGPUSurfaceTexture surfaceTexture;
      wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
      if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
         return nullptr;
      }

      WGPUTextureViewDescriptor viewDescriptor;
      viewDescriptor.nextInChain     = nullptr;
      viewDescriptor.label           = "Surface texture view";
      viewDescriptor.format          = wgpuTextureGetFormat(surfaceTexture.texture);
      viewDescriptor.dimension       = WGPUTextureViewDimension_2D;
      viewDescriptor.baseMipLevel    = 0;
      viewDescriptor.mipLevelCount   = 1;
      viewDescriptor.baseArrayLayer  = 0;
      viewDescriptor.arrayLayerCount = 1;
      viewDescriptor.aspect          = WGPUTextureAspect_All;
      WGPUTextureView targetView     = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

      return targetView;
   }

   // Draw a frame and handle events
   void MainLoop() {
      // Check whether the user clicked on the close button (and any other
      // mouse/key event, which we don't use so far)
      glfwPollEvents();

      // Get the next target texture view
      WGPUTextureView targetView = GetNextSurfaceTextureView();
      if (!targetView)
         return;

      // Create a command encoder for the draw call
      WGPUCommandEncoderDescriptor encoderDesc = {};
      encoderDesc.nextInChain                  = nullptr;
      encoderDesc.label                        = "My command encoder";
      WGPUCommandEncoder encoder               = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

      // Create the render pass that clears the screen with our color
      WGPURenderPassDescriptor renderPassDesc = {};
      renderPassDesc.nextInChain              = nullptr;

      // The attachment part of the render pass descriptor describes the target texture of the pass
      WGPURenderPassColorAttachment renderPassColorAttachment = {};
      renderPassColorAttachment.view                          = targetView;
      renderPassColorAttachment.resolveTarget                 = nullptr;
      renderPassColorAttachment.loadOp                        = WGPULoadOp_Clear;
      renderPassColorAttachment.storeOp                       = WGPUStoreOp_Store;
      renderPassColorAttachment.clearValue                    = WGPUColor{0.9, 0.1, 0.2, 1.0};
#ifndef WEBGPU_BACKEND_WGPU
      renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif // NOT WEBGPU_BACKEND_WGPU

      renderPassDesc.colorAttachmentCount   = 1;
      renderPassDesc.colorAttachments       = &renderPassColorAttachment;
      renderPassDesc.depthStencilAttachment = nullptr;
      renderPassDesc.timestampWrites        = nullptr;

      // Create the render pass and end it immediately (we only clear the screen but do not draw anything)
      WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
      wgpuRenderPassEncoderEnd(renderPass);
      wgpuRenderPassEncoderRelease(renderPass);

      // Finally encode and submit the render pass
      WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
      cmdBufferDescriptor.nextInChain                 = nullptr;
      cmdBufferDescriptor.label                       = "Command buffer";
      WGPUCommandBuffer command                       = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
      wgpuCommandEncoderRelease(encoder);

      std::cout << "Submitting command..." << std::endl;
      wgpuQueueSubmit(queue, 1, &command);
      wgpuCommandBufferRelease(command);
      std::cout << "Command submitted." << std::endl;

      // At the end of the frame
      wgpuTextureViewRelease(targetView);
#ifndef __EMSCRIPTEN__
      wgpuSurfacePresent(surface);
#endif

#if defined(WEBGPU_BACKEND_DAWN)
      wgpuDeviceTick(device);
#elif defined(WEBGPU_BACKEND_WGPU)
      wgpuDevicePoll(device, false, nullptr);
#endif
   }

   // Return true as long as the main loop should keep on running
   bool IsRunning() { return !glfwWindowShouldClose(window); }

private:
   GLFWwindow* window;
   WGPUDevice  device;
   WGPUQueue   queue;
   WGPUSurface surface;
};

int main(void) {

   Application app;

   // Not Emscripten-friendly
   if (!app.Initialize()) {
      return 1;
   }

   // Not emscripten-friendly
   while (app.IsRunning()) {
      app.MainLoop();
   }

   app.Terminate();

   return 0;


   /*
   const float TICKS_PER_SECOND = 3.0f;

   /* Initialize the library * /
   if (!glfwInit())
      return -1;

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // Get the primary monitor
   GLFWmonitor*       primaryMonitor = glfwGetPrimaryMonitor();
   const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

   // Create a fullscreen window
   GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "SpaceBoom", NULL, NULL);
   if (!window) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }

   glfwSetKeyCallback(window, key_callback);

   // Set the window icon
   std::string icon_path = Renderer::ResPath() + "images/Logo2.png";
   setWindowIcon(window, icon_path.c_str());

   /* Make the window's context current * /
   glfwMakeContextCurrent(window);

   glfwSwapInterval(1);

   if (glewInit() != GLEW_OK)
      std::cout << "Error!" << std::endl;

   std::cout << "current version of GL: " << glGetString(GL_VERSION) << std::endl;

   GLCall(glEnable(GL_BLEND));
   GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

   // Initialize ImGui
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   ImGui::StyleColorsDark();
   const char* glsl_version = "#version 330";
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);

   Renderer renderer(window, &io);

   World::LoadMap("maps/SpaceShip.txt");
   World::gameobjects.push_back(std::make_unique<Fog>());

   Input::currentTime       = glfwGetTime();
   double realTimeLastFrame = Input::currentTime;
   double lastTick          = Input::startTime;
   audio().Song.play();

   // -------------------
   // Main rendering loop
   // -------------------
   while (!glfwWindowShouldClose(window)) {
      double lastFrameTime = Input::currentTime;
      Input::deltaTime     = World::timeSpeed * (glfwGetTime() - realTimeLastFrame);
      Input::currentTime   = Input::currentTime + Input::deltaTime;
      realTimeLastFrame    = glfwGetTime();
      if (!World::settingTimeSpeed) {
         World::timeSpeed = zeno(World::timeSpeed, 1.0, 0.4);
         audio().Update(World::timeSpeed);

      } else {
         World::settingTimeSpeed = false;
      }

      // Set the viewport size
      auto [width, height] = renderer.WindowSize();
      glViewport(0, 0, (GLsizei)width, (GLsizei)height);

      auto gameobjects = World::get_gameobjects();

      renderer.Clear();
      Input::updateKeyStates(window);

      World::UpdateObjects();

      if (!World::ticksPaused()) {
         if (World::shouldTick) {
            World::TickObjects();
            lastTick          = Input::currentTime;
            World::shouldTick = false;
         } else if (lastTick + (1.0 / TICKS_PER_SECOND) <= Input::currentTime) {
            World::TickObjects();
            lastTick = lastTick + (1.0 / TICKS_PER_SECOND);
         }
      }

      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // Render all objects
      World::RenderObjects(renderer);

      // Render debug lines
      renderer.DrawDebug();

      // Performance info
      {
         ImGui::PushFont(renderer.jacquard12_small);
         ImGui::Begin("Performance Info");
         ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
         ImGui::End();
         ImGui::PopFont();
      }

      // Render ImGui
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
   }

   // Cleanup ImGui
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwTerminate();
   return 0;
   */
}
