#pragma once

#include <webgpu/webgpu.hpp>
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

// Background
// ============================================================

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
   BufferBinding<StarUniforms,                                                                   // Type of the buffer
                 wgpu::bothShaderStages(wgpu::ShaderStage::Vertex, wgpu::ShaderStage::Fragment), // Shader visibility
                 wgpu::BufferBindingType::Uniform,                                               // Buffer binding type
                 false // Buffer is not dynamically offset
                 >;
// ============================================================

// SquareObjects
// ============================================================
struct SquareObjectVertex {
   glm::vec2 position;
   glm::vec2 uv;

   SquareObjectVertex(glm::vec2 position, glm::vec2 uv)
      : position(position)
      , uv(uv) {}

   bool operator==(const SquareObjectVertex& other) const { return position == other.position && uv == other.uv; }
};

namespace std {
template <>
struct hash<SquareObjectVertex> {
   size_t operator()(const SquareObjectVertex& v) const {
      size_t h1 = std::hash<glm::vec2>{}(v.position);
      size_t h2 = std::hash<glm::vec2>{}(v.uv);
      return h1 ^ (h2 << 1);
   }
};
} // namespace std

struct SquareObjectVertexUniform {
   glm::mat4 u_MVP;
};

struct SquareObjectFragmentUniform {
   glm::vec4 u_Color;
};

using SquareObjectLayout = BindGroupLayout<
   BufferBinding<SquareObjectVertexUniform, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform, true>,
   BufferBinding<SquareObjectFragmentUniform, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform, true>,
   TextureBinding<wgpu::ShaderStage::Fragment, wgpu::TextureSampleType::Float, wgpu::TextureViewDimension::_2D>,
   SamplerBinding<wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::NonFiltering>>;
// ============================================================

// Line
// ============================================================

// Laid out with
// https://eliemichel.github.io/WebGPU-AutoLayout/
struct LineVertexUniform {
   glm::vec2 u_StartPos; // at byte offset 0
   glm::vec2 u_EndPos;   // at byte offset 8
   float     u_Width;    // at byte offset 16
   float     _pad0[3];
   glm::mat4 u_MVP; // at byte offset 32

   LineVertexUniform(glm::vec2 u_StartPos, glm::vec2 u_EndPos, float u_Width, glm::mat4 u_MVP)
      : u_StartPos(u_StartPos)
      , u_EndPos(u_EndPos)
      , u_Width(u_Width)
      , u_MVP(u_MVP) {}
};

struct LineFragmentUniform {
   glm::vec4 u_Color;
};

using LineVertex = glm::vec2;

using LineLayout = BindGroupLayout<
   BufferBinding<LineVertexUniform, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform, false>,    // vertex
   BufferBinding<LineFragmentUniform, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform, false> // fragment
   >;
// ============================================================

// Fog
// ============================================================
struct FogVertexUniform {
   glm::mat4 u_MVP; // Model-View-Projection matrix at byte offset 0

   FogVertexUniform(glm::mat4 u_MVP)
      : u_MVP(u_MVP) {}
};


struct FogFragmentUniform {
   glm::vec4 u_Color;         // at byte offset 0
   glm::vec4 u_BandColor;     // at byte offset 16
   glm::vec2 uPlayerPosition; // at byte offset 32
   float     _pad0[2];

   FogFragmentUniform(glm::vec4 u_Color, glm::vec4 u_BandColor, glm::vec2 uPlayerPosition)
      : u_Color(u_Color)
      , u_BandColor(u_BandColor)
      , uPlayerPosition(uPlayerPosition) {}
};

using FogVertexUniformBinding = BufferBinding<FogVertexUniform,                 // Type of the buffer
                                              wgpu::ShaderStage::Vertex,        // Shader visibility
                                              wgpu::BufferBindingType::Uniform, // Buffer binding type
                                              false                             // Buffer is not dynamically offset
                                              >;

// Fragment Uniform Buffer Binding for Fog Shader
using FogFragmentUniformBinding = BufferBinding<FogFragmentUniform,               // Type of the buffer
                                                wgpu::ShaderStage::Fragment,      // Shader visibility
                                                wgpu::BufferBindingType::Uniform, // Buffer binding type
                                                true                              // Buffer is dynamically offset
                                                >;

using FogLayout = BindGroupLayout<FogVertexUniformBinding, FogFragmentUniformBinding>;
using FogVertex = glm::vec2;
// ============================================================

// Particles
// ============================================================
struct Particle {
   glm::vec2 position;
   glm::vec2 velocity;
   glm::vec4 color;

   typedef InstanceBufferLayout<glm::vec2> Layout;
};

struct ParticleVertex {
   glm::vec2 position;

   bool operator==(const ParticleVertex& other) const { return position == other.position; }
};

struct ParticleVertexUniform {
   glm::mat4 u_MVP;

   bool operator==(const ParticleVertexUniform& other) const { return u_MVP == other.u_MVP; }
};

namespace std {
template <>
struct hash<ParticleVertex> {
   size_t operator()(const ParticleVertex& v) const {
      size_t h1 = std::hash<glm::vec2>{}(v.position);
      return h1;
   }
};
} // namespace std

struct ParticleWorldInfo {
   float     deltaTime; // at byte offset 0
   float     _pad0;
   glm::vec2 mousePos; // at byte offset 8

   ParticleWorldInfo(float deltaTime, glm::vec2 mousePos)
      : deltaTime(deltaTime)
      , mousePos(mousePos) {}
};

using ParticleLayout = BindGroupLayout<
   BufferBinding<ParticleVertexUniform, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform, true>>;
using ParticleComputeLayout = BindGroupLayout<
   BufferBinding<Particle, wgpu::ShaderStage::Compute, wgpu::BufferBindingType::Storage, false>,
   BufferBinding<ParticleWorldInfo, wgpu::ShaderStage::Compute, wgpu::BufferBindingType::Uniform, true>>;
