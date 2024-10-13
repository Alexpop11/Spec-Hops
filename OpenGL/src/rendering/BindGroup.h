#pragma once

#include <webgpu/webgpu.hpp>

static int32_t created_bind_groups = 0;
class BindGroup {
public:
   int32_t id;
   BindGroup(wgpu::BindGroup bindGroup)
      : id(created_bind_groups++)
      , bindGroup(bindGroup) {}

   wgpu::BindGroup get() { return bindGroup; }

private:
   wgpu::BindGroup bindGroup;
};
