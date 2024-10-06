#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "glm/glm.hpp"

/*
class RenderPipelineWrapper {
public:
   RenderPipelineWrapper(wgpu::Device device, Shader& shader, wgpu::PrimitiveTopology topology,
                         wgpu::TextureFormat colorFormat);
   ~RenderPipelineWrapper();

   // Deleted copy constructor and assignment operator
   RenderPipelineWrapper(const RenderPipelineWrapper&)            = delete;
   RenderPipelineWrapper& operator=(const RenderPipelineWrapper&) = delete;

   // Move constructor and assignment operator
   RenderPipelineWrapper(RenderPipelineWrapper&& other) noexcept;
   RenderPipelineWrapper& operator=(RenderPipelineWrapper&& other) noexcept;

   wgpu::RenderPipeline GetPipeline() const { return pipeline; }

private:
   wgpu::Device         device;
   wgpu::RenderPipeline pipeline;
};
*/
