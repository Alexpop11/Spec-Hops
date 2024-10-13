#pragma once

#include <webgpu/webgpu.hpp>

class CommandEncoder;

class RenderPass {
public:
   explicit RenderPass(CommandEncoder& encoder);
   ~RenderPass();

   wgpu::RenderPassEncoder& get();

private:
   wgpu::RenderPassEncoder renderPass_;
   wgpu::TextureView       targetView_;
};
