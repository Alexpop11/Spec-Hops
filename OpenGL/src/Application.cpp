#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

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

#define STB_IMAGE_IMPLEMENTATION // for icon
#include "stb_image.h"

#include <glfw3webgpu.h>
#include "rendering/webgpu-utils.h"
#include "rendering/Shader.h"
#include "rendering/RenderPipeline.h"
#include "rendering/VertexBufferLayout.h"

#include "glm/glm.hpp"

#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
   #include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// We embed the source of the shader module here
const char* shaderSource = R"(
/**
 * A structure with fields labeled with vertex attribute locations can be used
 * as input to the entry point of a shader.
 */
struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color: vec3f,
};

/**
 * A structure with fields labeled with builtins and locations can also be used
 * as *output* of the vertex shader, which is also the input of the fragment
 * shader.
 */
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec3f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput; // create the output struct
    out.position = vec4f(in.position, 0.0, 1.0); // same as what we used to directly return
    out.color = in.color; // forward the color attribute to the fragment shader
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let linear_color = pow(in.color, vec3f(2.2));
    return vec4f(linear_color, 1.0);
}
)";

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
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   auto window = glfwCreateWindow(640, 480, "Spec Hops", nullptr, nullptr);

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
   requiredLimits.limits.maxBufferSize = 6 * 5 * sizeof(float);
   // Maximum stride between 2 consecutive vertices in the vertex buffer
   requiredLimits.limits.maxVertexBufferArrayStride = 5 * sizeof(float);

   // These two limits are different because they are "minimum" limits,
   // they are the only ones we are may forward from the adapter's supported
   // limits.
   requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
   requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;

   return requiredLimits;
}

wgpu::Device getDevice(wgpu::Adapter& adapter) {
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
   });
}

RenderPipeline initializePipeline(wgpu::Device& device, wgpu::TextureFormat& surfaceFormat) {
   // Load the shader module
   Shader             shader(device, shaderSource);
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   // Create the render pipeline
   wgpu::RenderPipelineDescriptor pipelineDesc;

   VertexBufferLayout<glm::vec2, glm::vec3> layout;
   auto                                     vertexBufferInfo = layout.CreateLayout();

   std::vector<VertexBufferInfo> layouts;
   layouts.push_back(std::move(vertexBufferInfo));

   return RenderPipeline(device, shader, layouts, wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

// Initialize everything and return true if it went all right
Application::Application()
   : window(createWindow())
   , instance(wgpuCreateInstance(nullptr))
   , surface(glfwCreateWindowWGPUSurface(instance, window))
   , adapter(getAdapter(instance, surface))
   , device(getDevice(adapter))
   , uncapturedErrorCallbackHandle(getUncapturedErrorCallbackHandle(device))
   , queue(device.getQueue())
   , surfaceFormat(surface.getPreferredFormat(adapter))
   , pipeline(initializePipeline(device, surfaceFormat)) {

   // Configure the surface
   wgpu::SurfaceConfiguration config = {};

   // Configuration of the textures created for the underlying swap chain
   config.width  = 640;
   config.height = 480;
   config.usage  = wgpu::TextureUsage::RenderAttachment;
   config.format = surfaceFormat;

   // And we do not need any particular view format:
   config.viewFormatCount = 0;
   config.viewFormats     = nullptr;
   config.device          = device;
   config.presentMode     = wgpu::PresentMode::Fifo;
   config.alphaMode       = wgpu::CompositeAlphaMode::Auto;

   surface.configure(config);

   InitializeBuffers();

   InitializeResPath();

   initialized = true;
}

void Application::InitializeBuffers() {
   // Define point data
   // The de-duplicated list of point positions
   std::vector<float> pointData = {
      // x,   y,     r,   g,   b
      -0.5, -0.5, 1.0, 0.5, 0.5, // Point #0
      +0.5, -0.5, 0.5, 1.0, 0.5, // Point #1
      +0.5, +0.5, 0.5, 0.5, 1.0, // Point #2
      -0.5, +0.5, 1.0, 1.0, 0.5  // Point #3
   };

   // Define index data
   // This is a list of indices referencing positions in the pointData
   std::vector<uint16_t> indexData = {
      0, 1, 2, // Triangle #0 connects points #0, #1 and #2
      0, 2, 3  // Triangle #1 connects points #0, #2 and #3
   };

   indexCount = static_cast<uint32_t>(indexData.size());

   // Create vertex buffer
   wgpu::BufferDescriptor bufferDesc;
   bufferDesc.size             = pointData.size() * sizeof(float);
   bufferDesc.usage            = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex; // Vertex usage here!
   bufferDesc.mappedAtCreation = false;
   pointBuffer                 = device.createBuffer(bufferDesc);

   // Upload geometry data to the buffer
   queue.writeBuffer(pointBuffer, 0, pointData.data(), bufferDesc.size);

   // Create index buffer
   // (we reuse the bufferDesc initialized for the pointBuffer)
   bufferDesc.size  = indexData.size() * sizeof(uint16_t);
   bufferDesc.size  = (bufferDesc.size + 3) & ~3; // round up to the next multiple of 4
   bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
   indexBuffer      = device.createBuffer(bufferDesc);

   queue.writeBuffer(indexBuffer, 0, indexData.data(), bufferDesc.size);
}

void Application::InitializeResPath() {
   namespace fs = std::filesystem;

   if (res_path.empty()) {
      // Get the path of the executable
      fs::path exe_path = fs::weakly_canonical(fs::path("/proc/self/exe"));
      fs::path exe_dir  = exe_path.parent_path();

      // Check for "res/shaders" relative to the executable's directory
      if (fs::exists(exe_dir / "res" / "shaders")) {
         res_path = (exe_dir / "res").string() + "/";
      } else if (fs::exists(RES_PATH "/res/shaders")) {
         res_path = RES_PATH "/res/";
      } else {
         std::cout << "Resource directory not found relative to " << exe_dir << " or " << RES_PATH << std::endl;
         res_path = "./res/";
      }
   }
}


// Uninitialize everything that was initialized
void Application::Terminate() {
   pointBuffer.release();
   indexBuffer.release();

   surface.unconfigure();
   queue.release();
   surface.release();
   device.release();
   glfwDestroyWindow(window);
   glfwTerminate();
}

// Draw a frame and handle events
void Application::MainLoop() {
   glfwPollEvents();
   // Get the next target texture view
   wgpu::TextureView targetView = GetNextSurfaceTextureView();
   if (!targetView)
      return;

   // Create a command encoder for the draw call
   wgpu::CommandEncoderDescriptor encoderDesc = {};
   encoderDesc.label                          = "My command encoder";
   wgpu::CommandEncoder encoder               = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

   // Create the render pass that clears the screen with our color
   wgpu::RenderPassDescriptor renderPassDesc = {};

   // The attachment part of the render pass descriptor describes the target texture of the pass
   wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
   renderPassColorAttachment.view                            = targetView;
   renderPassColorAttachment.resolveTarget                   = nullptr;
   renderPassColorAttachment.loadOp                          = wgpu::LoadOp::Clear;
   renderPassColorAttachment.storeOp                         = wgpu::StoreOp::Store;
   renderPassColorAttachment.clearValue                      = WGPUColor{0.1, 0.1, 0.1, 1.0};
#ifndef WEBGPU_BACKEND_WGPU
   renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif // NOT WEBGPU_BACKEND_WGPU

   renderPassDesc.colorAttachmentCount   = 1;
   renderPassDesc.colorAttachments       = &renderPassColorAttachment;
   renderPassDesc.depthStencilAttachment = nullptr;
   renderPassDesc.timestampWrites        = nullptr;

   wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);

   // Select which render pipeline to use
   renderPass.setPipeline(pipeline.GetPipeline());
   // Set vertex buffer while encoding the render pass
   renderPass.setVertexBuffer(0, pointBuffer, 0, pointBuffer.getSize());

   // The second argument must correspond to the choice of uint16_t or uint32_t
   // we've done when creating the index buffer.
   renderPass.setIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint16, 0, indexBuffer.getSize());


   // We use the `vertexCount` variable instead of hard-coding the vertex count
   renderPass.drawIndexed(indexCount, 1, 0, 0, 0);


   renderPass.end();
   renderPass.release();

   // Finally encode and submit the render pass
   wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
   cmdBufferDescriptor.label                         = "Command buffer";
   wgpu::CommandBuffer command                       = encoder.finish(cmdBufferDescriptor);
   encoder.release();

   std::cout << "Submitting command..." << std::endl;
   queue.submit(1, &command);
   command.release();
   std::cout << "Command submitted." << std::endl;

   // At the enc of the frame
   targetView.release();
#ifndef __EMSCRIPTEN__
   surface.present();
#endif

#if defined(WEBGPU_BACKEND_DAWN)
   device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
   device.poll(false);
#endif
}

// Return true as long as the main loop should keep on running
bool Application::IsRunning() {
   return !glfwWindowShouldClose(window);
}

Application& Application::get() {
   static Application application = Application(); // Initialized first time this function is called
   return application;
}


wgpu::TextureView Application::GetNextSurfaceTextureView() {
   wgpu::SurfaceTexture surfaceTexture;
   surface.getCurrentTexture(&surfaceTexture);
   if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
      return nullptr;
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

   return targetView;
}

int main(void) {

   Application& app = Application::get();

   // Not Emscripten-friendly
   if (!app.initialized) {
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
