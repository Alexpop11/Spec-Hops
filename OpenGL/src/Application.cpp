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

int main(void) {
   // We create a descriptor
   WGPUInstanceDescriptor desc = {};
   desc.nextInChain            = nullptr;

   // By default Dawn runs callbacks only when the device “ticks”, so the error callbacks are invoked in a different
   // call stack than where the error occurred, making the breakpoint less informative. To force Dawn to invoke error
   // callbacks as soon as there is an error, we enable an instance toggle:
#ifdef WEBGPU_BACKEND_DAWN
   // Make sure the uncaptured error callback is called as soon as an error
   // occurs rather than at the next call to "wgpuDeviceTick".
   WGPUDawnTogglesDescriptor toggles;
   toggles.chain.next          = nullptr;
   toggles.chain.sType         = WGPUSType_DawnTogglesDescriptor;
   toggles.disabledToggleCount = 0;
   toggles.enabledToggleCount  = 1;
   const char* toggleName      = "enable_immediate_error_handling";
   toggles.enabledToggles      = &toggleName;

   desc.nextInChain = &toggles.chain;
#endif // WEBGPU_BACKEND_DAWN


   // We create the instance using this descriptor
   WGPUInstance instance = wgpuCreateInstance(&desc);

   // We can check whether there is actually an instance created
   if (!instance) {
      std::cerr << "Could not initialize WebGPU!" << std::endl;
      return 1;
   }

   // Display the object (WGPUInstance is a simple pointer, it may be
   // copied around without worrying about its size).
   std::cout << "WGPU instance: " << instance << std::endl;

   std::cout << "Requesting adapter..." << std::endl;

   WGPURequestAdapterOptions adapterOpts = {};
   adapterOpts.nextInChain               = nullptr;
   WGPUAdapter adapter                   = requestAdapterSync(instance, &adapterOpts);

   std::cout << "Got adapter: " << adapter << std::endl;

#ifndef __EMSCRIPTEN__
   WGPUSupportedLimits supportedLimits = {};
   supportedLimits.nextInChain         = nullptr;

   #ifdef WEBGPU_BACKEND_DAWN
   bool success = wgpuAdapterGetLimits(adapter, &supportedLimits) == WGPUStatus_Success;
   #else
   bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);
   #endif

   if (success) {
      std::cout << "Adapter limits:" << std::endl;
      std::cout << " - maxTextureDimension1D: " << supportedLimits.limits.maxTextureDimension1D << std::endl;
      std::cout << " - maxTextureDimension2D: " << supportedLimits.limits.maxTextureDimension2D << std::endl;
      std::cout << " - maxTextureDimension3D: " << supportedLimits.limits.maxTextureDimension3D << std::endl;
      std::cout << " - maxTextureArrayLayers: " << supportedLimits.limits.maxTextureArrayLayers << std::endl;
   }
#endif // NOT __EMSCRIPTEN__

   std::vector<WGPUFeatureName> features;

   // Call the function a first time with a null return address, just to get
   // the entry count.
   size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);

   // Allocate memory (could be a new, or a malloc() if this were a C program)
   features.resize(featureCount);

   // Call the function a second time, with a non-null return address
   wgpuAdapterEnumerateFeatures(adapter, features.data());

   std::cout << "Adapter features:" << std::endl;
   std::cout << std::hex; // Write integers as hexadecimal to ease comparison with webgpu.h literals
   for (auto f : features) {
      std::cout << " - 0x" << f << std::endl;
   }
   std::cout << std::dec; // Restore decimal numbers

   // Get the adapter properties
   WGPUAdapterInfo info = {};
   info.nextInChain     = nullptr;
   wgpuAdapterGetInfo(adapter, &info);
   std::cout << "Adapter info:" << std::endl;
   std::cout << " - vendorID: " << info.vendorID << std::endl;
   if (info.vendor) {
      std::cout << " - vendor: " << info.vendor << std::endl;
   }
   if (info.architecture) {
      std::cout << " - architecture: " << info.architecture << std::endl;
   }
   std::cout << " - deviceID: " << info.deviceID << std::endl;
   if (info.device) {
      std::cout << " - name: " << info.device << std::endl;
   }
   if (info.description) {
      std::cout << " - description: " << info.description << std::endl;
   }
   std::cout << std::hex;
   std::cout << " - adapterType: 0x" << info.adapterType << std::endl;
   std::cout << " - backendType: 0x" << info.backendType << std::endl;
   std::cout << std::dec; // Restore decimal numbers

   // Get a device
   std::cout << "Requesting device..." << std::endl;

   WGPUDeviceDescriptor deviceDesc     = {};
   deviceDesc.nextInChain              = nullptr;
   deviceDesc.label                    = "My Device"; // anything works here, that's your call
   deviceDesc.requiredFeatureCount     = 0;           // we do not require any specific feature
   deviceDesc.requiredLimits           = nullptr;     // we do not require any specific limit
   deviceDesc.defaultQueue.nextInChain = nullptr;
   deviceDesc.defaultQueue.label       = "The default queue";
   // A function that is invoked whenever the device stops being available.
   // Important! The deviceLostCallback must outlive the device, so that when the latter gets destroyed the callback is
   // still valid. Keep this in mind when refactoring later.
   deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
      std::cout << "Device lost: reason " << reason;
      if (message)
         std::cout << " (" << message << ")";
      std::cout << std::endl;
   };

   // [...] Build device descriptor
   WGPUDevice device = requestDeviceSync(adapter, &deviceDesc);

   inspectDevice(device);

   std::cout << "Got device: " << device << std::endl;

   // The uncaptured error callback is invoked whenever we misuse the API, and gives very informative feedback about
   // what went wrong.
   auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
      // If you use a debugger, put a breakpoint in this callback.
      std::cout << "Uncaptured device error: type " << type;
      if (message)
         std::cout << " (" << message << ")";
      std::cout << std::endl;
   };
   wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);


   // Release the device
   wgpuDeviceRelease(device);

   // Release the adapter now that it is no longer needed.
   wgpuAdapterRelease(adapter);


   // We clean up the WebGPU instance
   wgpuInstanceRelease(instance);


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
