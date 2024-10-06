// RenderPipeline.cpp
#include "RenderPipeline.h"

/*
RenderPipelineWrapper::RenderPipelineWrapper(wgpu::Device device, Shader& shader,
                                             const std::vector<VertexBufferLayout>& vertexLayouts,
                                             wgpu::PrimitiveTopology topology, wgpu::TextureFormat colorFormat)
   : device(device) {
   // Create vertex buffer layouts
   std::vector<wgpu::VertexBufferLayout> wgpuVertexLayouts;
   for (const auto& layout : vertexLayouts) {
      wgpu::VertexBufferLayout wgpuLayout = {};
      wgpuLayout.arrayStride              = layout.arrayStride;
      wgpuLayout.stepMode                 = layout.stepMode;

      std::vector<wgpu::VertexAttribute> wgpuAttributes;
      for (const auto& attr : layout.attributes) {
         wgpu::VertexAttribute wgpuAttr = {};
         wgpuAttr.shaderLocation        = attr.shaderLocation;
         wgpuAttr.format                = attr.format;
         wgpuAttr.offset                = attr.offset;
         wgpuAttributes.push_back(wgpuAttr);
      }

      wgpuLayout.attributeCount = static_cast<uint32_t>(wgpuAttributes.size());
      wgpuLayout.attributes     = wgpuAttributes.data();
      wgpuVertexLayouts.push_back(wgpuLayout);
   }

   // Define pipeline layout
   wgpu::PipelineLayoutDescriptor pipelineLayoutDesc = {};
   pipelineLayoutDesc.label                          = "Pipeline Layout";
   wgpu::PipelineLayout pipelineLayout               = device.createPipelineLayout(&pipelineLayoutDesc);

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
   pipelineDesc.layout                             = pipelineLayout;
   pipelineDesc.vertex                             = vertexState;
   pipelineDesc.fragment                           = &fragmentState;
   pipelineDesc.primitive                          = primitiveState;
   pipelineDesc.depthStencil                       = nullptr; // No depth/stencil
   pipelineDesc.multisample.count                  = 1;
   pipelineDesc.multisample.mask                   = ~0u;
   pipelineDesc.multisample.alphaToCoverageEnabled = false;

   // Create render pipeline
   pipeline = device.createRenderPipeline(&pipelineDesc);

   // Release the pipeline layout as it's no longer needed after pipeline creation
   pipelineLayout.release();
}

RenderPipelineWrapper::~RenderPipelineWrapper() {
   if (pipeline) {
      pipeline.release();
   }
}

RenderPipelineWrapper::RenderPipelineWrapper(RenderPipelineWrapper&& other) noexcept
   : device(other.device)
   , pipeline(other.pipeline) {
   other.pipeline = nullptr;
}

RenderPipelineWrapper& RenderPipelineWrapper::operator=(RenderPipelineWrapper&& other) noexcept {
   if (this != &other) {
      if (pipeline) {
         pipeline.release();
      }
      device         = other.device;
      pipeline       = other.pipeline;
      other.pipeline = nullptr;
   }
   return *this;
}
*/
