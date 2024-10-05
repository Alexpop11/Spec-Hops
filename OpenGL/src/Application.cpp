#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

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

using namespace wgpu;

// We embed the source of the shader module here
const char* shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
	var p = vec2f(0.0, 0.0);
	if (in_vertex_index == 0u) {
		p = vec2f(-0.5, -0.5);
	} else if (in_vertex_index == 1u) {
		p = vec2f(0.5, -0.5);
	} else {
		p = vec2f(0.0, 0.5);
	}
	return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
	return vec4f(0.0, 0.4, 1.0, 1.0);
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
      window = glfwCreateWindow(640, 480, "Spec Hops", nullptr, nullptr);

      std::string icon_path = Renderer::ResPath() + "Images/Logo2.png";
      setWindowIcon(window, icon_path.c_str());


      wgpu::Instance instance = wgpuCreateInstance(nullptr);

      surface = glfwGetWGPUSurface(instance, window);

      std::cout << "Requesting adapter..." << std::endl;
      surface                                 = glfwGetWGPUSurface(instance, window);
      wgpu::RequestAdapterOptions adapterOpts = {};
      adapterOpts.compatibleSurface           = surface;
      wgpu::Adapter adapter                   = instance.requestAdapter(adapterOpts);
      std::cout << "Got adapter: " << adapter << std::endl;

      PrintAdapterInfo(adapter);

      instance.release();

      std::cout << "Requesting device..." << std::endl;
      wgpu::DeviceDescriptor deviceDesc   = {};
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
      device = adapter.requestDevice(deviceDesc);
      std::cout << "Got device: " << device << std::endl;

      uncapturedErrorCallbackHandle = device.setUncapturedErrorCallback([](wgpu::ErrorType type, char const* message) {
         std::cout << "Uncaptured device error: type " << type;
         if (message)
            std::cout << " (" << message << ")";
         std::cout << std::endl;
      });

      queue = device.getQueue();

      // Configure the surface
      wgpu::SurfaceConfiguration config = {};

      // Configuration of the textures created for the underlying swap chain
      config.width  = 640;
      config.height = 480;
      config.usage  = wgpu::TextureUsage::RenderAttachment;
      surfaceFormat = surface.getPreferredFormat(adapter);
      config.format = surfaceFormat;

      // And we do not need any particular view format:
      config.viewFormatCount = 0;
      config.viewFormats     = nullptr;
      config.device          = device;
      config.presentMode     = wgpu::PresentMode::Fifo;
      config.alphaMode       = wgpu::CompositeAlphaMode::Auto;

      surface.configure(config);

      // Release the adapter only after it has been fully utilized
      adapter.release();

      InitializePipeline();

      return true;
   }

   void PrintAdapterInfo(wgpu::Adapter& adapter) {
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

   void InitializePipeline() {
      // Load the shader module
      wgpu::ShaderModuleDescriptor shaderDesc;
#ifdef WEBGPU_BACKEND_WGPU
      shaderDesc.hintCount = 0;
      shaderDesc.hints     = nullptr;
#endif

      // We use the extension mechanism to specify the WGSL part of the shader module descriptor
      wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
      // Set the chained struct's header
      shaderCodeDesc.chain.next = nullptr;
      // I changed this from ShaderModuleWGSLDescriptor to ShaderSourceWGSL
      shaderCodeDesc.chain.sType = wgpu::SType::ShaderSourceWGSL;
      // Connect the chain
      shaderDesc.nextInChain          = &shaderCodeDesc.chain;
      shaderCodeDesc.code             = shaderSource;
      wgpu::ShaderModule shaderModule = device.createShaderModule(shaderDesc);

      // Create the render pipeline
      wgpu::RenderPipelineDescriptor pipelineDesc;

      // We do not use any vertex buffer for this first simplistic example
      pipelineDesc.vertex.bufferCount = 0;
      pipelineDesc.vertex.buffers     = nullptr;

      // NB: We define the 'shaderModule' in the second part of this chapter.
      // Here we tell that the programmable vertex shader stage is described
      // by the function called 'vs_main' in that module.
      pipelineDesc.vertex.module        = shaderModule;
      pipelineDesc.vertex.entryPoint    = "vs_main";
      pipelineDesc.vertex.constantCount = 0;
      pipelineDesc.vertex.constants     = nullptr;

      // Each sequence of 3 vertices is considered as a triangle
      pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;

      // We'll see later how to specify the order in which vertices should be
      // connected. When not specified, vertices are considered sequentially.
      pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;

      // The face orientation is defined by assuming that when looking
      // from the front of the face, its corner vertices are enumerated
      // in the counter-clockwise (CCW) order.
      pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;

      // But the face orientation does not matter much because we do not
      // cull (i.e. "hide") the faces pointing away from us (which is often
      // used for optimization).
      pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

      // We tell that the programmable fragment shader stage is described
      // by the function called 'fs_main' in the shader module.
      wgpu::FragmentState fragmentState;
      fragmentState.module        = shaderModule;
      fragmentState.entryPoint    = "fs_main";
      fragmentState.constantCount = 0;
      fragmentState.constants     = nullptr;

      wgpu::BlendState blendState;
      blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
      blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
      blendState.color.operation = wgpu::BlendOperation::Add;
      blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
      blendState.alpha.dstFactor = wgpu::BlendFactor::One;
      blendState.alpha.operation = wgpu::BlendOperation::Add;

      wgpu::ColorTargetState colorTarget;
      colorTarget.format    = surfaceFormat;
      colorTarget.blend     = &blendState;
      colorTarget.writeMask = wgpu::ColorWriteMask::All; // We could write to only some of the color channels.

      // We have only one target because our render pass has only one output color
      // attachment.
      fragmentState.targetCount = 1;
      fragmentState.targets     = &colorTarget;
      pipelineDesc.fragment     = &fragmentState;

      // We do not use stencil/depth testing for now
      pipelineDesc.depthStencil = nullptr;

      // Samples per pixel
      pipelineDesc.multisample.count = 1;

      // Default value for the mask, meaning "all bits on"
      pipelineDesc.multisample.mask = ~0u;

      // Default value as well (irrelevant for count = 1 anyways)
      pipelineDesc.multisample.alphaToCoverageEnabled = false;
      pipelineDesc.layout                             = nullptr;

      pipeline = device.createRenderPipeline(pipelineDesc);

      // We no longer need to access the shader module
      shaderModule.release();
   }

   // Uninitialize everything that was initialized
   void Terminate() {
      pipeline.release();
      surface.unconfigure();
      queue.release();
      surface.release();
      device.release();
      glfwDestroyWindow(window);
      glfwTerminate();
   }

   wgpu::TextureView GetNextSurfaceTextureView() {
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

   // Draw a frame and handle events
   void MainLoop() {
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
      renderPassColorAttachment.clearValue                      = WGPUColor{0.9, 0.1, 0.2, 1.0};
#ifndef WEBGPU_BACKEND_WGPU
      renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif // NOT WEBGPU_BACKEND_WGPU

      renderPassDesc.colorAttachmentCount   = 1;
      renderPassDesc.colorAttachments       = &renderPassColorAttachment;
      renderPassDesc.depthStencilAttachment = nullptr;
      renderPassDesc.timestampWrites        = nullptr;

      wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);

      // Select which render pipeline to use
      renderPass.setPipeline(pipeline);
      // Draw 1 instance of a 3-vertices shape
      renderPass.draw(3, 1, 0, 0);

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
   bool IsRunning() { return !glfwWindowShouldClose(window); }

private:
   GLFWwindow*                          window;
   wgpu::Device                         device;
   wgpu::Queue                          queue;
   wgpu::Surface                        surface;
   std::unique_ptr<wgpu::ErrorCallback> uncapturedErrorCallbackHandle;
   wgpu::TextureFormat                  surfaceFormat = wgpu::TextureFormat::Undefined;
   wgpu::RenderPipeline                 pipeline;
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
