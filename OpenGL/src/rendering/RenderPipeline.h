#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"

template <typename BindGroupType>
class RenderPipeline;

template <BindingC... Bindings>
class RenderPipeline<BindGroupLayout<Bindings...>> {
public:
   RenderPipeline(const std::string& label, wgpu::Device& device, Shader& shader,
                  const std::vector<VertexBufferInfo>& vertexInfos, wgpu::PrimitiveTopology topology,
                  wgpu::TextureFormat colorFormat)
      : device(device)
      , bindGroupLayout(BindGroupLayout<Bindings...>::CreateLayout(device)) {
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

   wgpu::BindGroup BindGroup(WGPUType<Bindings>&... resources) {
      // Create a tuple of references to the resources
      auto resourcesTuple = std::forward_as_tuple(resources...);

      // Create the bind group using the resources
      wgpu::BindGroup bindGroup = createBindGroup(resourcesTuple, std::index_sequence_for<Bindings...>{});

      return bindGroup;
   }

   template <typename Tuple, size_t... I>
   wgpu::BindGroup createBindGroup(Tuple& resources, std::index_sequence<I...>) {
      // Create an array of BindGroupEntry
      std::array<wgpu::BindGroupEntry, sizeof...(Bindings)> entries = {
         createBindGroupEntry<I, Bindings>(std::get<I>(resources))...};

      // Create the BindGroupDescriptor
      wgpu::BindGroupDescriptor bindGroupDesc{};
      bindGroupDesc.layout     = bindGroupLayout;
      bindGroupDesc.entryCount = static_cast<uint32_t>(entries.size());
      bindGroupDesc.entries    = entries.data();

      // Create and return the BindGroup
      return device.createBindGroup(bindGroupDesc);
   }

   template <size_t I, typename Binding, typename Resource>
   wgpu::BindGroupEntry createBindGroupEntry(Resource& resource) const {
      wgpu::BindGroupEntry entry{};
      entry.binding = static_cast<uint32_t>(I);
      if constexpr (Binding::bindingType == BindingType::Buffer && !Binding::dynamicOffset) {
         // Assuming WGPUType<T> is Buffer
         entry.buffer = std::get<0>(resource).get();
         entry.offset = std::get<1>(resource);
         entry.size   = sizeof(typename Binding::Type);
      } else if constexpr (Binding::bindingType == BindingType::Buffer && Binding::dynamicOffset) {
         entry.buffer = resource.get();
         entry.offset = 0;
         entry.size   = sizeof(typename Binding::Type);

      } else if constexpr (Binding::bindingType == BindingType::Sampler) {
         // Assuming WGPUType<T> is wgpu::Sampler
         entry.sampler = resource.Get(); // Replace Get() with actual method to retrieve the sampler handle
      } else if constexpr (Binding::bindingType == BindingType::Texture) {
         // Assuming WGPUType<T> is wgpu::TextureView
         entry.textureView = resource.Get(); // Replace Get() with actual method to retrieve the texture view handle
      }
      return entry;
   }


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
   wgpu::Device          device;
   wgpu::RenderPipeline  pipeline;
   wgpu::BindGroupLayout bindGroupLayout;
};
