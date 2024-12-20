#include "CommandEncoder.h"
#include "DeadBuffers.h"
#include "Application.h"
#include <iostream>

CommandEncoder::CommandEncoder(const wgpu::Device& device)
   : device_(device) {
   wgpu::CommandEncoderDescriptor encoderDesc = {};
   encoderDesc.label                          = "My command encoder";
   encoder_                                   = device_.createCommandEncoder(encoderDesc);
}

CommandEncoder::~CommandEncoder() {
   // Finish encoding and submit the command buffer
   wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
   cmdBufferDescriptor.label                         = "Command buffer";
   wgpu::CommandBuffer command                       = encoder_.finish();

   auto& application = Application::get();
   auto  queue       = application.getQueue();

   queue.submit(1, &command);

   encoder_.release();
   command.release();
}

wgpu::CommandEncoder& CommandEncoder::get() {
   return encoder_;
}

void CommandEncoder::DestroyDeadBuffers() {
   for (auto& buffer : DeadBuffers::buffers) {
      buffer.destroy();
      buffer.release();
   }
   DeadBuffers::buffers.clear();
}
