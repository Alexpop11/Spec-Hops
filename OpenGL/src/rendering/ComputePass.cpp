#include "ComputePass.h"
#include "CommandEncoder.h"
#include "Application.h"


ComputePass::ComputePass(CommandEncoder& encoder, wgpu::TextureView& targetView) {
   auto& application = Application::get();

   wgpu::ComputePassDescriptor computePassDesc;
   computePass_ = encoder.get().beginComputePass(computePassDesc);
}

ComputePass::~ComputePass() {
   computePass_.end();
   computePass_.release();
}

wgpu::ComputePassEncoder& ComputePass::get() {
   return computePass_;
}
