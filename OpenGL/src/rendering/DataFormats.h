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



class SquareObjectVertex {
   glm::vec2 position;
   glm::vec2 uv;
};

class SquareObjectVertexUniform {
   glm::mat4 u_MVP;
};

class SquareObjectFragmentUniform {
   glm::vec4 u_Color;
};

using SquareObjectUniformLayout = BindGroupLayout<
   BufferBinding<SquareObjectVertexUniform, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform, true>,
   BufferBinding<SquareObjectFragmentUniform, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform, true>,
   TextureBinding<wgpu::ShaderStage::Fragment, wgpu::TextureSampleType::Float, wgpu::TextureViewDimension::_2D>,
   SamplerBinding<wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::NonFiltering>>;
