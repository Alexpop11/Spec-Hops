#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"
#include "DataFormats.h"

template <typename BGLs, typename VBL>
class RenderPipeline {
public:
   RenderPipeline(const std::string& label, wgpu::Device& device, Shader& shader, wgpu::PrimitiveTopology topology,
                  wgpu::TextureFormat colorFormat)
      : device(device)
      , bindGroupLayouts(BGLs::CreateLayouts(device)) {
      // Create vertex buffer layouts
      auto vertexInfo = VBL::CreateLayout();

      std::vector<wgpu::VertexBufferLayout> wgpuVertexLayouts;
      wgpuVertexLayouts.push_back(vertexInfo.layout);

      wgpu::PipelineLayoutDescriptor layoutDesc{};
      layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
      layoutDesc.bindGroupLayouts     = (WGPUBindGroupLayout*)bindGroupLayouts.data();
      layoutDesc.label                = label.c_str();
      wgpu::PipelineLayout layout     = device.createPipelineLayout(layoutDesc);

      // Define color target state (to enable blending)
      wgpu::BlendComponent blendColor    = {};
      blendColor.srcFactor               = wgpu::BlendFactor::SrcAlpha;
      blendColor.dstFactor               = wgpu::BlendFactor::OneMinusSrcAlpha;
      blendColor.operation               = wgpu::BlendOperation::Add;
      wgpu::BlendComponent blendAlpha    = {};
      blendAlpha.srcFactor               = wgpu::BlendFactor::One;
      blendAlpha.dstFactor               = wgpu::BlendFactor::OneMinusSrcAlpha;
      blendAlpha.operation               = wgpu::BlendOperation::Add;
      wgpu::BlendState blendState        = {};
      blendState.color                   = blendColor;
      blendState.alpha                   = blendAlpha;
      wgpu::ColorTargetState colorTarget = {};
      colorTarget.format                 = colorFormat;
      colorTarget.blend                  = &blendState; // Enable blending
      colorTarget.writeMask              = wgpu::ColorWriteMask::All;


      // Define fragment state
      wgpu::FragmentState fragmentState = {};
      fragmentState.module              = shader.GetShaderModule();
      fragmentState.entryPoint          = "fragment_main";
      fragmentState.targetCount         = 1;
      fragmentState.targets             = &colorTarget;

      // Define vertex state
      wgpu::VertexState vertexState = {};
      vertexState.module            = shader.GetShaderModule();
      vertexState.entryPoint        = "vertex_main";
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
      , bindGroupLayouts(other.bindGroupLayouts) {
      other.pipeline         = nullptr;
      other.bindGroupLayouts = nullptr;
   };
   RenderPipeline& operator=(RenderPipeline&& other) noexcept {
      if (this != &other) {
         if (pipeline) {
            pipeline.release();
         }
         pipeline         = other.pipeline;
         bindGroupLayouts = other.bindGroupLayouts;
         other.pipeline   = nullptr;
      }
      return *this;
   };

   template <typename... Ts>
   auto BindGroups(Ts&&... ts) {
      return BGLs::BindGroups(device, std::forward<Ts>(ts)...);
   }

   wgpu::RenderPipeline               GetPipeline() const { return pipeline; }
   std::vector<wgpu::BindGroupLayout> GetBindGroupLayouts() const { return bindGroupLayouts; }

private:
   wgpu::Device                       device;
   wgpu::RenderPipeline               pipeline;
   std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
};
