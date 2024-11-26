#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"
#include "BindGroupLayout.h"
#include "DataFormats.h"
#include "Id.h"

static int32_t created_render_pipelines = 0;

template <typename BGLs>
class ComputePipeline {
public:
   ComputePipeline(std::filesystem::path shaderPath)
      : id(Id::get())
      , device(Application::get().getDevice())
      , bindGroupLayouts(BGLs::CreateLayouts(device)) {
      std::string label = shaderPath.stem().string();
      Shader      shader(device, shaderPath);

      wgpu::PipelineLayoutDescriptor layoutDesc{};
      layoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
      layoutDesc.bindGroupLayouts     = (WGPUBindGroupLayout*)bindGroupLayouts.data();
      layoutDesc.label                = label.c_str();
      wgpu::PipelineLayout layout     = device.createPipelineLayout(layoutDesc);

      // Define pipeline descriptor
      wgpu::ComputePipelineDescriptor computePipelineDesc = wgpu::Default;
      computePipelineDesc.compute.constantCount           = 0;
      computePipelineDesc.compute.constants               = nullptr;
      computePipelineDesc.compute.entryPoint              = "compute_main";
      computePipelineDesc.compute.module                  = shader.GetShaderModule();
      computePipelineDesc.layout                          = layout;

      // Create compute pipeline
      pipeline = device.createComputePipeline(computePipelineDesc);
   };

   ~ComputePipeline() {
      if (pipeline) {
         pipeline.release();
      }
   };

   // Deleted copy constructor and assignment operator
   ComputePipeline(const ComputePipeline&)             = delete;
   ComputePipeline& operator=(const ComputePipeline&)  = delete;
   ComputePipeline(ComputePipeline&& other)            = delete;
   ComputePipeline& operator=(ComputePipeline&& other) = delete;

   template <typename... Ts>
   auto BindGroups(Ts&&... ts) {
      return BGLs::BindGroups(device, std::forward<Ts>(ts)...);
   }

   wgpu::ComputePipeline              GetPipeline() const { return pipeline; }
   std::vector<wgpu::BindGroupLayout> GetBindGroupLayouts() const { return bindGroupLayouts; }

   const int32_t id;

private:
   wgpu::Device                       device;
   wgpu::ComputePipeline              pipeline;
   std::vector<wgpu::BindGroupLayout> bindGroupLayouts;
};
