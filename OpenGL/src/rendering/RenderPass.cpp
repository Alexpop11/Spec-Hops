#include "RenderPass.h"
#include "CommandEncoder.h"
#include "Application.h"


RenderPass::RenderPass(CommandEncoder& encoder, wgpu::TextureView& targetView) {
   auto& application = Application::get();

   // Set up the render pass descriptor
   wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
   renderPassColorAttachment.view                            = targetView;
   renderPassColorAttachment.resolveTarget                   = nullptr;
   renderPassColorAttachment.loadOp                          = wgpu::LoadOp::Clear;
   renderPassColorAttachment.storeOp                         = wgpu::StoreOp::Store;
   renderPassColorAttachment.clearValue                      = WGPUColor{0.1, 0.1, 0.1, 1.0};
#ifndef WEBGPU_BACKEND_WGPU
   renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif // NOT WEBGPU_BACKEND_WGPU

   wgpu::RenderPassDescriptor renderPassDesc = {};
   renderPassDesc.colorAttachmentCount       = 1;
   renderPassDesc.colorAttachments           = &renderPassColorAttachment;
   renderPassDesc.depthStencilAttachment     = nullptr;
   renderPassDesc.timestampWrites            = nullptr;

   renderPass_ = encoder.get().beginRenderPass(renderPassDesc);
}

RenderPass::~RenderPass() {
   renderPass_.end();
   renderPass_.release();
}

wgpu::RenderPassEncoder& RenderPass::get() {
   return renderPass_;
}
