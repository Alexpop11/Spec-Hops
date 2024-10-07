#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"


class RenderPipeline {
public:
   RenderPipeline(const std::string& label, wgpu::Device& device, Shader& shader,
                  const std::vector<VertexBufferInfo>& vertexInfos, const wgpu::BindGroupLayout bindGroupLayout,
                  wgpu::PrimitiveTopology topology, wgpu::TextureFormat colorFormat);
   ~RenderPipeline();

   // Deleted copy constructor and assignment operator
   RenderPipeline(const RenderPipeline&)            = delete;
   RenderPipeline& operator=(const RenderPipeline&) = delete;

   // Move constructor and assignment operator
   RenderPipeline(RenderPipeline&& other) noexcept;
   RenderPipeline& operator=(RenderPipeline&& other) noexcept;

   wgpu::RenderPipeline  GetPipeline() const { return pipeline; }
   wgpu::BindGroupLayout GetBindGroupLayout() const { return bindGroupLayout; }

private:
   wgpu::RenderPipeline  pipeline;
   wgpu::BindGroupLayout bindGroupLayout;
};
