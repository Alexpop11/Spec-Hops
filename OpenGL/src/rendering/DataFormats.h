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

   StarUniforms(float time, glm::vec2 resolution)
      : time(time)
      , resolution(resolution) {}
};

using StarUniformBinding =
   BufferBinding<StarUniforms,                                                       // Type of the buffer
                 wgpu::both(wgpu::ShaderStage::Vertex, wgpu::ShaderStage::Fragment), // Shader visibility
                 wgpu::BufferBindingType::Uniform,                                   // Buffer binding type
                 true>;



struct SquareObjectVertex {
   glm::vec2 position;
   glm::vec2 uv;

   SquareObjectVertex(glm::vec2 position, glm::vec2 uv)
      : position(position)
      , uv(uv) {}
};

struct SquareObjectVertexUniform {
   glm::mat4 u_MVP;
};

struct SquareObjectFragmentUniform {
   glm::vec4 u_Color;
};

using SquareObjectLayout = BindGroupLayout<
   BufferBinding<SquareObjectVertexUniform, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform, false>,
   BufferBinding<SquareObjectFragmentUniform, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform, false>,
   TextureBinding<wgpu::ShaderStage::Fragment, wgpu::TextureSampleType::Float, wgpu::TextureViewDimension::_2D>,
   SamplerBinding<wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::NonFiltering>>;
