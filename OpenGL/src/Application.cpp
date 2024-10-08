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
#include "rendering/Buffer.h"
#include "rendering/BindGroupLayout.h"

#include "glm/glm.hpp"

#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
   #include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// We embed the source of the shader module here
const char* shaderSource = R"(
struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
}

@vertex 
fn vs_main(@location(0) position: vec4<f32>) -> VertexOutput {
    return VertexOutput(position);
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
}

struct StarUniforms {
	time: f32,
	resolution: vec2f,
};

@group(0) @binding(0) var<uniform> uSTarUniforms: StarUniforms;
var<private> color: vec4<f32>;
var<private> gl_FragCoord_1: vec4<f32>;

fn random(coord: vec2<f32>) -> f32 {
    var coord_1: vec2<f32>;

    coord_1 = coord;
    let _e9 = coord_1;
    let _e18 = coord_1;
    let _e30 = coord_1;
    let _e39 = coord_1;
    return fract((sin(dot(_e39, vec2<f32>(12.9898f, 78.233f))) * 43758.547f));
}

fn star(uv: vec2<f32>, center: vec2<f32>, size: f32) -> f32 {
    var uv_1: vec2<f32>;
    var center_1: vec2<f32>;
    var size_1: f32;
    var d: vec2<f32>;
    var manhattanDist: f32;
    var euclideanDist: f32;
    var pointiness: f32 = 3f;
    var star_1: f32;
    var flareR: f32;
    var invertedflareR: f32;
    var flare: f32;

    uv_1 = uv;
    center_1 = center;
    size_1 = size;
    let _e9 = uv_1;
    let _e10 = center_1;
    let _e12 = uv_1;
    let _e13 = center_1;
    d = abs((_e12 - _e13));
    let _e17 = d;
    let _e19 = d;
    manhattanDist = (_e17.x + _e19.y);
    let _e24 = d;
    euclideanDist = length(_e24);
    let _e32 = size_1;
    let _e37 = size_1;
    let _e40 = euclideanDist;
    star_1 = (1f - smoothstep(0f, (_e37 / 1.5f), _e40));
    let _e46 = pointiness;
    let _e48 = manhattanDist;
    let _e49 = euclideanDist;
    let _e50 = pointiness;
    flareR = mix(_e48, _e49, -(_e50));
    let _e54 = size_1;
    let _e55 = flareR;
    invertedflareR = (_e54 - _e55);
    let _e58 = size_1;
    let _e63 = size_1;
    let _e68 = flareR;
    flare = smoothstep((_e63 + 0.01f), 0f, _e68);
    let _e73 = flare;
    let _e74 = star_1;
    return max(_e73, _e74);
}

fn starColor(seed: f32) -> vec3<f32> {
    var seed_1: f32;
    var colorType: f32;

    seed_1 = seed;
    let _e5 = seed_1;
    let _e8 = seed_1;
    let _e11 = random(vec2<f32>(_e8, 0.4f));
    colorType = _e11;
    let _e13 = colorType;
    if (_e13 < 0.79f) {
        {
            return vec3<f32>(1f, 1f, 1f);
        }
    } else {
        let _e20 = colorType;
        if (_e20 < 0.82f) {
            {
                return vec3<f32>(0.65f, 0.73f, 1f);
            }
        } else {
            let _e27 = colorType;
            if (_e27 < 0.9f) {
                {
                    return vec3<f32>(1f, 0.75f, 0.75f);
                }
            } else {
                {
                    return vec3<f32>(1f, 0.9f, 0.7f);
                }
            }
        }
    }
}

fn main_1() {
    var uv_2: vec2<f32>;
    var aspectRatio: f32;
    var numStars: i32 = 200i;
    var finalColor: vec3<f32> = vec3(0f);
    var i: i32 = 0i;
    var seed_2: f32;
    var speedF: f32;
    var local: f32;
    var local_1: f32;
    var starSpeed: f32;
    var starSize: f32;
    var starY: f32;
    var starX: f32;
    var starPos: vec2<f32>;
    var starTint: vec3<f32>;
    var smallStarBrightness: f32;

    let _e4 = gl_FragCoord_1;
    let _e6 = uSTarUniforms.resolution;
    uv_2 = (_e4.xy / _e6.xy);
    let _e10 = uSTarUniforms.resolution;
    let _e12 = uSTarUniforms.resolution;
    aspectRatio = (_e10.x / _e12.y);
    let _e17 = uv_2;
    let _e19 = aspectRatio;
    uv_2.x = (_e17.x * _e19);
    let _e22 = uv_2;
    let _e27 = uv_2;
    uv_2.x = (floor((_e27.x * 450f)) / 450f);
    let _e37 = uv_2;
    let _e42 = uv_2;
    uv_2.y = (floor((_e42.y * 450f)) / 450f);
    loop {
        let _e58 = i;
        let _e59 = numStars;
        if !((_e58 < _e59)) {
            break;
        }
        {
            let _e65 = i;
            let _e67 = numStars;
            seed_2 = (f32(_e65) / f32(_e67));
            let _e71 = seed_2;
            let _e74 = seed_2;
            let _e77 = random(vec2<f32>(_e74, 0.1f));
            speedF = _e77;
            let _e79 = speedF;
            if (_e79 < 0.995f) {
                let _e82 = speedF;
                if (_e82 < 0.975f) {
                    let _e90 = speedF;
                    local = mix(0.01f, 0.08f, _e90);
                } else {
                    local = 0.23f;
                }
                let _e94 = local;
                local_1 = _e94;
            } else {
                local_1 = 0.8f;
            }
            let _e97 = local_1;
            starSpeed = -(_e97);
            let _e102 = seed_2;
            let _e105 = seed_2;
            let _e108 = random(vec2<f32>(_e105, 0.2f));
            let _e111 = seed_2;
            let _e114 = seed_2;
            let _e117 = random(vec2<f32>(_e114, 0.2f));
            starSize = mix(0.001f, 0.005f, _e117);
            let _e120 = seed_2;
            let _e123 = seed_2;
            let _e126 = random(vec2<f32>(_e123, 0.3f));
            starY = _e126;
            let _e128 = seed_2;
            let _e129 = uSTarUniforms.time;
            let _e130 = starSpeed;
            let _e133 = seed_2;
            let _e134 = uSTarUniforms.time;
            let _e135 = starSpeed;
            starX = fract((_e133 + (_e134 * _e135)));
            let _e140 = starX;
            let _e141 = aspectRatio;
            let _e143 = starY;
            starPos = vec2<f32>((_e140 * _e141), _e143);
            let _e147 = seed_2;
            let _e148 = starColor(_e147);
            starTint = _e148;
            let _e150 = finalColor;
            let _e151 = starTint;
            let _e155 = uv_2;
            let _e156 = starPos;
            let _e157 = starSize;
            let _e158 = star(_e155, _e156, _e157);
            finalColor = (_e150 + (_e151 * _e158));
        }
        continuing {
            let _e62 = i;
            i = (_e62 + 1i);
        }
    }
    let _e162 = uv_2;
    let _e164 = aspectRatio;
    let _e167 = uv_2;
    let _e171 = uv_2;
    let _e173 = aspectRatio;
    let _e176 = uv_2;
    let _e179 = random(vec2<f32>((0.05f + (_e171.x / _e173)), _e176.y));
    smallStarBrightness = _e179;
    let _e181 = smallStarBrightness;
    if (_e181 > 0.985f) {
        {
            let _e184 = finalColor;
            finalColor = (_e184 + vec3(0.3f));
        }
    }
    let _e188 = finalColor;
    color = vec4<f32>(_e188.x, _e188.y, _e188.z, 1f);
    return;
}

@fragment 
fn fs_main(@builtin(position) gl_FragCoord: vec4<f32>) -> FragmentOutput {
    gl_FragCoord_1 = gl_FragCoord;
    main_1();
    let _e9 = color;
    return FragmentOutput(_e9);
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

#ifdef _MSC_VER
      __debugbreak();
#else
      assert(false);
#endif
   });
}

RenderPipeline<BindGroupLayout<StarUniformBinding>> initializePipeline(wgpu::Device&        device,
                                                                       wgpu::TextureFormat& surfaceFormat) {
   // Load the shader module
   Shader             shader(device, shaderSource);
   wgpu::ShaderModule shaderModule = shader.GetShaderModule();

   // Create the render pipeline
   wgpu::RenderPipelineDescriptor pipelineDesc;

   VertexBufferLayout<glm::vec2> layout;
   auto                          vertexBufferInfo = layout.CreateLayout();

   std::vector<VertexBufferInfo> layouts;
   layouts.push_back(std::move(vertexBufferInfo));

   return RenderPipeline<BindGroupLayout<StarUniformBinding>>("Stars", device, shader, layouts,
                                                              wgpu::PrimitiveTopology::TriangleList, surfaceFormat);
}

wgpu::TextureFormat preferredFormat(wgpu::Surface& surface, wgpu::Adapter& adapter) {
   wgpu::SurfaceCapabilities capabilities;
   surface.getCapabilities(adapter, &capabilities);
   return capabilities.formats[0];
}

Buffer<float> getPointBuffer(wgpu::Device& device, wgpu::Queue& queue) {
   std::vector<float> pointData = {
      // x,   y,
      -1, -1, +1, -1, +1, +1, -1, +1,
   };
   return Buffer<float>(device, queue, pointData, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex);
}

Buffer<uint16_t> getIndexBuffer(wgpu::Device& device, wgpu::Queue& queue) {
   std::vector<uint16_t> indexData = {
      0, 1, 2, // Triangle #0 connects points #0, #1 and #2
      0, 2, 3  // Triangle #1 connects points #0, #2 and #3
   };
   return Buffer<uint16_t>(device, queue, indexData, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index);
}

UniformBuffer<StarUniforms> getUniformBuffer(wgpu::Device& device, wgpu::Queue& queue) {
   StarUniforms uniform;
   uniform.time       = 1.0f;
   uniform.resolution = {640, 480};
   StarUniforms uniform2;
   uniform2.time       = 2.0f;
   uniform2.resolution = {640, 480};


   std::vector<StarUniforms> uniformData = {uniform, uniform2};
   return Buffer<StarUniforms, true>(device, queue, uniformData,
                                     wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
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
   , surfaceFormat(preferredFormat(surface, adapter))
   , pipeline(initializePipeline(device, surfaceFormat))
   , pointBuffer(getPointBuffer(device, queue))
   , indexBuffer(getIndexBuffer(device, queue))
   , uniformBuffer(getUniformBuffer(device, queue)) {

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


   InitializeResPath();

   initialized = true;
}


// Draw a frame and handle events
void Application::MainLoop() {
   glfwPollEvents();
   // Get the next target texture view
   wgpu::TextureView targetView = GetNextSurfaceTextureView();
   if (!targetView)
      return;

   // Update the uniform buffer
   StarUniforms uniform;
   uniform.time       = glfwGetTime();
   uniform.resolution = {640, 480};
   StarUniforms uniform2;
   uniform2.time       = -glfwGetTime();
   uniform2.resolution = {640, 480};

   std::vector<StarUniforms> uniformData = {uniform, uniform2};
   uniformBuffer.upload(uniformData);

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

   wgpu::BindGroup bindGroup = pipeline.BindGroup(uniformBuffer);

   renderPass.setPipeline(pipeline.GetPipeline());
   renderPass.setVertexBuffer(0, pointBuffer.get(), 0, pointBuffer.size());
   renderPass.setIndexBuffer(indexBuffer.get(), wgpu::IndexFormat::Uint16, 0, indexBuffer.size());

   uint32_t dynamicOffset = 0;
   dynamicOffset          = uniformBuffer.index(0);
   renderPass.setBindGroup(0, bindGroup, 1, &dynamicOffset);
   renderPass.drawIndexed(indexBuffer.count(), 1, 0, 0, 0);

   dynamicOffset = uniformBuffer.index(1);
   renderPass.setBindGroup(0, bindGroup, 1, &dynamicOffset);
   renderPass.drawIndexed(indexBuffer.count(), 1, 0, 0, 0);


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
