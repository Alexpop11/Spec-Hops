#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"
#include "DataFormats.h"

static int32_t created_render_pipelines = 0;

template <typename BGLs, typename VBLs>
class RenderPipeline {
public:
   RenderPipeline(std::filesystem::path   shaderPath,
                  wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::TriangleList)
      : id(created_render_pipelines++)
      , device(Application::get().getDevice())
      , bindGroupLayouts(BGLs::CreateLayouts(device)) {
      std::string label = shaderPath.stem().string();
      Shader      shader(device, shaderPath);

      // Create vertex buffer layouts
      auto vertexInfos = VBLs::CreateLayouts();

      std::vector<wgpu::VertexBufferLayout> wgpuVertexLayouts;
      for (const auto& info : vertexInfos) {
         wgpuVertexLayouts.push_back(info.layout);
      }

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
      colorTarget.format                 = Application::get().getSurfaceFormat();
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
      pipelineDesc.label                              = label.c_str();
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
   RenderPipeline(const RenderPipeline&)             = delete;
   RenderPipeline& operator=(const RenderPipeline&)  = delete;
   RenderPipeline(RenderPipeline&& other)            = delete;
   RenderPipeline& operator=(RenderPipeline&& other) = delete;

   template <typename... Ts>
   auto BindGroups(Ts&&... ts) {
      return BGLs::BindGroups(device, std::forward<Ts>(ts)...);
   }

   wgpu::RenderPipeline               GetPipeline() const { return pipeline; }
   std::vector<wgpu::BindGroupLayout> GetBindGroupLayouts() const { return bindGroupLayouts; }

   const int32_t id;

private:
   wgpu::Device                       device;
   wgpu::RenderPipeline               pipeline;
   std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
};
