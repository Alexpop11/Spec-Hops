#pragma once

#include <webgpu/webgpu.hpp>
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"

// Laid out with
// https://eliemichel.github.io/WebGPU-AutoLayout/
struct StarUniforms {
   float     time; // at byte offset 0
   float     _pad0;
   glm::vec2 resolution; // at byte offset 8
};

using StarUniformBinding =
   BufferBinding<StarUniforms,                                                       // Type of the buffer
                 wgpu::both(wgpu::ShaderStage::Vertex, wgpu::ShaderStage::Fragment), // Shader visibility
                 wgpu::BufferBindingType::Uniform,                                   // Buffer binding type
                 true>;
