#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"
template <typename... T>
class RenderPipeline {
public:
   RenderPipeline(const std::string& label, wgpu::Device& device, Shader& shader,
                  const std::vector<VertexBufferInfo>& vertexInfos, wgpu::PrimitiveTopology topology,
                  wgpu::TextureFormat colorFormat)
      : bindGroupLayout(BindGroupLayoutGenerator<T...>::CreateLayout(device)) {
      // Create vertex buffer layouts
      std::vector<wgpu::VertexBufferLayout> wgpuVertexLayouts;
      for (const auto& info : vertexInfos) {
         wgpuVertexLayouts.push_back(info.layout);
      }

      wgpu::PipelineLayoutDescriptor layoutDesc{};
      layoutDesc.bindGroupLayoutCount = 1;
      layoutDesc.bindGroupLayouts     = (WGPUBindGroupLayout*)&bindGroupLayout;
      layoutDesc.label                = label.c_str();
      wgpu::PipelineLayout layout     = device.createPipelineLayout(layoutDesc);

      // Define color target state
      wgpu::ColorTargetState colorTarget = {};
      colorTarget.format                 = colorFormat;
      colorTarget.blend                  = nullptr; // You can set up blending here if needed
      colorTarget.writeMask              = wgpu::ColorWriteMask::All;

      // Define fragment state
      wgpu::FragmentState fragmentState = {};
      fragmentState.module              = shader.GetShaderModule();
      fragmentState.entryPoint          = "fs_main";
      fragmentState.targetCount         = 1;
      fragmentState.targets             = &colorTarget;

      // Define vertex state
      wgpu::VertexState vertexState = {};
      vertexState.module            = shader.GetShaderModule();
      vertexState.entryPoint        = "vs_main";
      vertexState.bufferCount       = static_cast<uint32_t>(wgpuVertexLayouts.size());
      vertexState.buffers           = wgpuVertexLayouts.data();

      // Define primitive state
      wgpu::PrimitiveState primitiveState = {};
      primitiveState.topology             = topology;
      primitiveState.stripIndexFormat     = wgpu::IndexFormat::Undefined;
      primitiveState.frontFace            = wgpu::FrontFace::CCW;
      primitiveState.cullMode             = wgpu::CullMode::None;

      // Define pipeline descriptor
      wgpu::RenderPipelineDescriptor pipelineDesc     = {};
      pipelineDesc.label                              = "Render Pipeline";
      pipelineDesc.layout                             = layout;
      pipelineDesc.vertex                             = vertexState;
      pipelineDesc.fragment                           = &fragmentState;
      pipelineDesc.primitive                          = primitiveState;
      pipelineDesc.depthStencil                       = nullptr; // No depth/stencil
      pipelineDesc.multisample.count                  = 1;
      pipelineDesc.multisample.mask                   = ~0u;
      pipelineDesc.multisample.alphaToCoverageEnabled = false;


      // Create render pipeline
      pipeline = device.createRenderPipeline(pipelineDesc);
   };

   ~RenderPipeline() {
      if (pipeline) {
         pipeline.release();
      }
   };

   // Deleted copy constructor and assignment operator
   RenderPipeline(const RenderPipeline&)            = delete;
   RenderPipeline& operator=(const RenderPipeline&) = delete;

   // Move constructor and assignment operator
   RenderPipeline(RenderPipeline&& other) noexcept
      : pipeline(other.pipeline)
      , bindGroupLayout(other.bindGroupLayout) {
      other.pipeline        = nullptr;
      other.bindGroupLayout = nullptr;
   };
   RenderPipeline& operator=(RenderPipeline&& other) noexcept {
      if (this != &other) {
         if (pipeline) {
            pipeline.release();
         }
         pipeline        = other.pipeline;
         bindGroupLayout = other.bindGroupLayout;
         other.pipeline  = nullptr;
      }
      return *this;
   };

   wgpu::RenderPipeline  GetPipeline() const { return pipeline; }
   wgpu::BindGroupLayout GetBindGroupLayout() const { return bindGroupLayout; }

private:
   wgpu::RenderPipeline  pipeline;
   wgpu::BindGroupLayout bindGroupLayout;
};
