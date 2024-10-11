#pragma once

#include <webgpu/webgpu.hpp>

class CommandEncoder {
public:
   explicit CommandEncoder(const wgpu::Device& device);
   ~CommandEncoder();

   wgpu::CommandEncoder& get();

private:
   wgpu::Device         device_;
   wgpu::CommandEncoder encoder_;
};
