#pragma once
#include <webgpu/webgpu.hpp>
#include <filesystem>

#include "imgui.h"
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

   GLFWwindow*          getWindow() { return window; }
   wgpu::TextureFormat& getSurfaceFormat() { return surfaceFormat; }
   wgpu::Device&        getDevice() { return device; }
   wgpu::Surface&       getSurface() { return surface; }
   wgpu::Queue&         getQueue() { return queue; }
   ImGuiIO&             getImGuiIO() { return io; }
   glm::vec2            MousePos();

   wgpu::TextureView GetNextSurfaceTextureView();
   void              onResize();
   void              configureSurface();
   glm::ivec2        windowSize();

private:
   GLFWwindow*                          window;
   wgpu::Instance                       instance;
   wgpu::Surface                        surface;
   wgpu::Adapter                        adapter;
   wgpu::Device                         device;
   std::unique_ptr<wgpu::ErrorCallback> uncapturedErrorCallbackHandle;
   wgpu::Queue                          queue;
   wgpu::TextureFormat                  surfaceFormat;
   ImGuiIO&                             io;

public:
   // Fonts
   ImFont* jacquard12_big;
   ImFont* jacquard12_small;
   ImFont* pixelify;
};
