#include "RenderPass.h"
#include "CommandEncoder.h"
#include "Application.h"

#include "glm/gtc/matrix_transform.hpp"

RenderPass::RenderPass(CommandEncoder& encoder) {
   auto& application = Application::get();

   // Get the next target texture view
   targetView_ = application.GetNextSurfaceTextureView();
   if (!targetView_) {
      // Handle the error appropriately
      throw std::runtime_error("Failed to get target texture view.");
   }

   // Set up the render pass descriptor
   wgpu::RenderPassColorAttachment renderPassColorAttachment = {};
   renderPassColorAttachment.view                            = targetView_;
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
   // Resources will be automatically released when going out of scope
}

wgpu::RenderPassEncoder& RenderPass::get() {
   return renderPass_;
}


glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale) {
   glm::ivec2 windowSize = Application::get().windowSize();

   // Calculate aspect ratio
   float aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);

   // Create orthographic projection matrix
   float     orthoWidth = 18 * aspectRatio;
   glm::mat4 projection = glm::ortho(-orthoWidth / 2.0f, orthoWidth / 2.0f, -18 / 2.0f, 18 / 2.0f, -1.0f, 1.0f);

   // Create view matrix
   glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-glm::vec2(1, 1), 0.0f));

   // Create model matrix
   glm::mat4 model           = glm::translate(glm::mat4(1.0f), glm::vec3(objectPosition, 0.0f));
   float     rotationRadians = glm::radians(objectRotationDegrees);
   model                     = glm::rotate(model, rotationRadians, glm::vec3(0.0f, 0.0f, 1.0f));
   model                     = glm::scale(model, glm::vec3(objectScale, objectScale, 1.0f));

   // Combine matrices to form MVP
   glm::mat4 mvp = projection * view * model;

   return mvp;
}
