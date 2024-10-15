#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#undef MINIAUDIO_IMPLEMENTATION

#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>
#undef WEBGPU_CPP_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#ifdef __EMSCRIPTEN__
   #include <emscripten.h>
#endif // __EMSCRIPTEN__

#include "Application.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>

#include "res_path.hpp"


#include <glfw3webgpu.h>
#include "rendering/webgpu-utils.h"
#include "rendering/Shader.h"
#include "rendering/RenderPipeline.h"
#include "rendering/VertexBufferLayout.h"
#include "rendering/Buffer.h"
#include "rendering/BindGroupLayout.h"
#include "rendering/Renderer.h"
#include "rendering/Texture.h"
#include "rendering/CommandEncoder.h"
#include "rendering/RenderPass.h"
#include "AudioEngine.h"

#include "glm/glm.hpp"

#include "game_objects/GameObject.h"
#include "game_objects/Background.h"
#include "game_objects/SquareObject.h"
#include "game_objects/Tile.h"
#include "game_objects/Fog.h"
#include "World.h"

// TODO: Not emscripten friendly, see https://github.com/ocornut/imgui/blob/master/examples/example_glfw_wgpu/main.cpp
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"

#ifdef __APPLE__
   #include <mach-o/dyld.h>
#elif defined(__linux__)
   #include <unistd.h>
#endif

const float TICKS_PER_SECOND = 3.0f;

// Called every frame
void mainLoop(Application& application, Renderer& renderer) {
   glfwPollEvents();
   auto device = application.getDevice();

   double lastFrameTime     = Input::currentTime;
   Input::deltaTime         = World::timeSpeed * (glfwGetTime() - Input::realTimeLastFrame);
   Input::currentTime       = Input::currentTime + Input::deltaTime;
   Input::realTimeLastFrame = glfwGetTime();
   Input::updateKeyStates(application.getWindow());
   if (!World::settingTimeSpeed) {
      World::timeSpeed = zeno(World::timeSpeed, 1.0, 0.4);
      audio().Update(World::timeSpeed);
   } else {
      World::settingTimeSpeed = false;
   }

   World::UpdateObjects();

   if (!World::ticksPaused()) {
      if (World::shouldTick) {
         World::TickObjects();
         Input::lastTick   = Input::currentTime;
         World::shouldTick = false;
      } else if (Input::lastTick + (1.0 / TICKS_PER_SECOND) <= Input::currentTime) {
         World::TickObjects();
         Input::lastTick = Input::lastTick + (1.0 / TICKS_PER_SECOND);
      }
   }

   auto nextTexture = application.GetNextSurfaceTextureView();
   if (nextTexture) {
      auto [targetView, targetTexture, surfaceTexture] = *nextTexture;
      {
         // Create a command encoder for the draw call
         CommandEncoder encoder(device);

         // Create the render pass
         RenderPass renderPass(encoder, targetView);
         renderer.renderPass = renderPass.get();

         // Start the Dear ImGui frame
         ImGui_ImplWGPU_NewFrame();
         ImGui_ImplGlfw_NewFrame();
         ImGui::NewFrame();

         World::RenderObjects(renderer, renderPass);
         renderer.DrawDebug(renderPass);

         // Performance info
         {
            ImGui::PushFont(application.pixelify);
            ImGui::Begin("Performance Info");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / application.getImGuiIO().Framerate,
                        application.getImGuiIO().Framerate);
            ImGui::End();
            ImGui::PopFont();
         }

         ImGui::Render();
         ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass.get());

         // The render pass and command encoder will be ended and submitted in their destructors
      }
      renderer.FinishFrame();
      targetView.release();
      wgpuTextureRelease(surfaceTexture.texture);
   } else {
      std::cout << "No next texture, cannot render." << std::endl;
   }

#ifndef __EMSCRIPTEN__
   application.getSurface().present();
#endif

#if defined(WEBGPU_BACKEND_DAWN)
   application.getDevice().tick();
#elif defined(WEBGPU_BACKEND_WGPU)
   application.getDevice().poll(false);
#endif
}

// Setup stuff
// ===========


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

GLFWwindow* createWindow() {
   glfwInit();
   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
   glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

   GLFWmonitor*       primaryMonitor = glfwGetPrimaryMonitor();
   const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

   // Create a fullscreen window
   std::cout << "Mode width: " << mode->width << " Mode height: " << mode->height << std::endl;
   // create a window that is at least 648x480
   auto window =
      glfwCreateWindow(std::max(mode->width, 648), std::max(mode->height, 480), "Spec Hops", nullptr, nullptr);

   return window;
}

void printAdapterInfo(wgpu::Adapter& adapter) {
   // Get the adapter properties
   wgpu::AdapterInfo info = {};
   info.nextInChain       = nullptr;
   adapter.getInfo(&info);
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
}

wgpu::Adapter getAdapter(wgpu::Instance& instance, wgpu::Surface& surface) {
   std::cout << "Requesting adapter..." << std::endl;
   wgpu::RequestAdapterOptions adapterOpts = {};
   adapterOpts.compatibleSurface           = surface;
   wgpu::Adapter adapter                   = instance.requestAdapter(adapterOpts);
   std::cout << "Got adapter: " << adapter << std::endl;
   printAdapterInfo(adapter);
   return adapter;
}

wgpu::RequiredLimits getRequiredLimits(wgpu::Adapter& adapter) {
   wgpu::SupportedLimits supportedLimits;
   adapter.getLimits(&supportedLimits);

   // Don't forget to = Default
   wgpu::RequiredLimits requiredLimits = wgpu::Default;

   // We use at most 2 vertex attribute for now
   requiredLimits.limits.maxVertexAttributes = 2;
   // We also never transfer more than 3 components between stages
   requiredLimits.limits.maxInterStageShaderComponents = 3;
   // We should also tell that we use 1 vertex buffers
   requiredLimits.limits.maxVertexBuffers = 1;
   // Maximum size of a buffer is 6 vertices of 2 float each
   requiredLimits.limits.maxBufferSize = 15 * 5 * sizeof(float);
   // Maximum stride between 2 consecutive vertices in the vertex buffer
   requiredLimits.limits.maxVertexBufferArrayStride = 5 * sizeof(float);
   // We use at most 1 uniform buffer per stage
   requiredLimits.limits.maxUniformBuffersPerShaderStage = 1;
   // Uniform structs have a size of maximum 16 float
   requiredLimits.limits.maxUniformBufferBindingSize = 16 * 4;
   // We use at most 1 bind group for now
   requiredLimits.limits.maxBindGroups = 1;


   // These two limits are different because they are "minimum" limits,
   // they are the only ones we are may forward from the adapter's supported
   // limits.
   requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
   requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;

   return requiredLimits;
}

wgpu::Device createDevice(wgpu::Adapter& adapter) {
   std::cout << "Requesting device..." << std::endl;
   wgpu::DeviceDescriptor deviceDesc   = {};
   deviceDesc.label                    = "My Device";
   deviceDesc.requiredFeatureCount     = 0;
   wgpu::RequiredLimits requiredLimits = getRequiredLimits(adapter);
   deviceDesc.requiredLimits           = &requiredLimits;
   deviceDesc.defaultQueue.nextInChain = nullptr;
   deviceDesc.defaultQueue.label       = "The default queue";
   deviceDesc.deviceLostCallback       = [](WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
      std::cout << "Device lost: reason " << reason;
      if (message)
         std::cout << " (" << message << ")";
      std::cout << std::endl;
   };
   auto device = adapter.requestDevice(deviceDesc);
   std::cout << "Got device: " << device << std::endl;
   return device;
}

auto getUncapturedErrorCallbackHandle(wgpu::Device& device) {
   return device.setUncapturedErrorCallback([](wgpu::ErrorType type, char const* message) {
      std::cout << "Uncaptured device error: type " << type;
      if (message)
         std::cout << " (" << message << ")";
      std::cout << std::endl;

#ifdef _MSC_VER
      __debugbreak();
#else
      assert(false);
#endif
   });
}

wgpu::TextureFormat preferredFormat(wgpu::Surface& surface, wgpu::Adapter& adapter) {
   wgpu::SurfaceCapabilities capabilities;
   surface.getCapabilities(adapter, &capabilities);
   return capabilities.formats[0];
}

glm::ivec2 Application::windowSize() {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   return {width, height};
}

void Application::configureSurface() {
   // Configure the surface
   wgpu::SurfaceConfiguration config = {};

   // Configuration of the textures created for the underlying swap chain
   auto windowSize = this->windowSize();
   std::cout << "Window size: " << windowSize.x << " " << windowSize.y << std::endl;

   config.width  = windowSize.x;
   config.height = windowSize.y;
   config.usage  = wgpu::TextureUsage::RenderAttachment;
   config.format = surfaceFormat;

   // And we do not need any particular view format:
   config.viewFormatCount = 0;
   config.viewFormats     = nullptr;
   config.device          = device;
   config.presentMode     = wgpu::PresentMode::Fifo;
   config.alphaMode       = wgpu::CompositeAlphaMode::Auto;

   surface.configure(config);
}

ImGuiIO& setUpImgui(wgpu::Device& device, GLFWwindow* window, wgpu::TextureFormat surfaceFormat) {
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   // ImGui::StyleColorsLight();

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOther(window, true);
#ifdef __EMSCRIPTEN__
   ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
   ImGui_ImplWGPU_InitInfo init_info;
   init_info.Device             = device;
   init_info.NumFramesInFlight  = 3;
   init_info.RenderTargetFormat = surfaceFormat;
   init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;
   ImGui_ImplWGPU_Init(&init_info);
   return io;
}

std::filesystem::path getExecutablePath() {
   namespace fs = std::filesystem;
   fs::path exe_path;

#ifdef __APPLE__
   char     pathbuf[1024];
   uint32_t size = sizeof(pathbuf);
   if (_NSGetExecutablePath(pathbuf, &size) != 0) {
      std::cerr << "Buffer too small; need size " << size << "\n";
      return fs::current_path(); // Fallback to current path
   }
   exe_path = fs::weakly_canonical(fs::path(pathbuf));
#elif defined(__linux__)
   char    buf[1024];
   ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
   if (len != -1) {
      buf[len] = '\0';
      exe_path = fs::weakly_canonical(fs::path(buf));
   } else {
      std::cerr << "Failed to read /proc/self/exe\n";
      return fs::current_path(); // Fallback to current path
   }
#else
   // Fallback for other platforms
   exe_path = fs::current_path();
#endif

   return exe_path;
}

std::filesystem::path getResPath() {
   namespace fs = std::filesystem;

   // Get the path of the executable
   fs::path exe_path = getExecutablePath();
   fs::path exe_dir  = exe_path.parent_path();

   std::filesystem::path res_path;

   // Check for "res/shaders" relative to the executable's directory
   if (fs::exists(exe_dir / "res" / "shaders")) {
      res_path = (exe_dir / "res").string() + "/";
   } else if (fs::exists(RES_PATH "/res/shaders")) {
      res_path = RES_PATH "/res/";
   } else {
      std::cout << "Resource directory not found relative to " << exe_dir << " or " << RES_PATH << std::endl;
      res_path = "./res/";
   }
   return res_path;
}

ImFont* load_font(std::filesystem::path res_path, ImGuiIO* io, const std::string& font_name, int size) {
   auto        font_path     = res_path / "fonts" / font_name;
   std::string font_path_str = font_path.string();
   ImFont*     font          = io->Fonts->AddFontFromFileTTF(font_path_str.c_str(), size);
   return font;
}

bool Application::initialized = false;

// Initialize everything and return true if it went all right
Application::Application()
   : res_path(getResPath())
   , window(createWindow())
   , instance(wgpuCreateInstance(nullptr))
   , surface(glfwCreateWindowWGPUSurface(instance, window))
   , adapter(getAdapter(instance, surface))
   , device(createDevice(adapter))
   , uncapturedErrorCallbackHandle(getUncapturedErrorCallbackHandle(device))
   , queue(device.getQueue())
   , surfaceFormat(preferredFormat(surface, adapter))
   , io(setUpImgui(device, window, surfaceFormat))
   , jacquard12_big(load_font(res_path, &io, "Jacquard12.ttf", 40))
   , jacquard12_small(load_font(res_path, &io, "Jacquard12.ttf", 18))
   , pixelify(load_font(res_path, &io, "PixelifySans.ttf", 16)) {
   glfwSetKeyCallback(window, key_callback);

   glfwSetWindowUserPointer(window, this);
   // Use a non-capturing lambda as resize callback
   glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int, int) {
      auto that = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
      if (that != nullptr)
         that->onResize();
   });
   configureSurface();

   std::filesystem::path icon_path     = res_path / "images" / "Logo2.png";
   std::string           icon_path_str = icon_path.string();
   setWindowIcon(window, icon_path_str.c_str());

   adapter.release();
   instance.release();

   if (initialized) {
      std::cout << "Application already initialized!" << std::endl;
      assert(false);
   } else {
      std::cout << "Application initialized" << std::endl;
   }

   initialized = true;
}

void Application::onResize() {
   configureSurface();
}

// Uninitialize everything that was initialized
void Application::Terminate() {
   surface.unconfigure();
   queue.release();
   surface.release();
   device.release();
   glfwDestroyWindow(window);
   glfwTerminate();
}

// Return true as long as the main loop should keep on running
bool Application::IsRunning() {
   return !glfwWindowShouldClose(window);
}

Application& Application::get() {
   static Application application = Application(); // Initialized first time this function is called
   return application;
}


std::optional<std::tuple<wgpu::TextureView, wgpu::Texture, wgpu::SurfaceTexture>>
Application::GetNextSurfaceTextureView() {
   surface.getCurrentTexture(&surfaceTexture);
   if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
      return {};
   }
   wgpu::Texture texture = surfaceTexture.texture;

   wgpu::TextureViewDescriptor viewDescriptor;
   viewDescriptor.label           = "Surface texture view";
   viewDescriptor.format          = texture.getFormat();
   viewDescriptor.dimension       = wgpu::TextureViewDimension::_2D;
   viewDescriptor.baseMipLevel    = 0;
   viewDescriptor.mipLevelCount   = 1;
   viewDescriptor.baseArrayLayer  = 0;
   viewDescriptor.arrayLayerCount = 1;
   viewDescriptor.aspect          = wgpu::TextureAspect::All;
   wgpu::TextureView targetView   = texture.createView(viewDescriptor);

   return std::make_tuple(targetView, texture, surfaceTexture);
}

int main(void) {
   Application& application = Application::get();
   Renderer     renderer    = Renderer();

   World::LoadMap("SpaceShip.txt");
   World::gameobjects.push_back(std::make_unique<Fog>());

   audio().Song.play();

   Input::currentTime       = glfwGetTime();
   Input::realTimeLastFrame = Input::currentTime;
   Input::lastTick          = Input::currentTime;
   // audio().Song.play();

   // Not Emscripten-friendly
   if (!application.initialized) {
      return 1;
   }

   // Not emscripten-friendly

#ifdef __EMSCRIPTEN__
   // Equivalent of the main loop when using Emscripten:
   auto callback = [](void* arg) {
      Renderer*    renderer    = reinterpret_cast<Renderer*>(arg);
      Application& application = Application::get();
      mainLoop(application, *renderer);
   };
   emscripten_set_main_loop_arg(callback, &renderer, 0, true);
#else
   // Equivalent of the main loop when using Emscripten:
   while (application.IsRunning()) {
      mainLoop(application, renderer);
   }
#endif

   application.Terminate();

   return 0;
}
