#pragma once

#include <webgpu/webgpu.hpp>
#include "Buffer.h"
#include "BindGroup.h"
#include "Texture.h"
#include "TextureSampler.h"

class CommandEncoder;

class RenderPass {
public:
   explicit RenderPass(CommandEncoder& encoder, wgpu::TextureView& targetView);
   ~RenderPass();

   wgpu::RenderPassEncoder& get();


   template <typename T>
   void setPipeline(const T& pass) {
      if (last_set_render_pipeline != pass.id) {
         renderPass_.setPipeline(pass.GetPipeline());
         last_set_render_pipeline = pass.id;
      }
   }

   void setBindGroup(uint32_t group, BindGroup bindGroup, std::vector<uint32_t> offset) {
      // TODO: this is overly conservative because we could be checking the that the bind group / offset is the same as
      // the last time we set that index
      if ((last_set_bind_group != (int32_t)group) ||           // Check index
          (bindGroup.id != (int32_t)last_set_bind_group_id) || // Check bind group id
          offset != last_set_bind_group_offset                 // check offset
      ) {
         renderPass_.setBindGroup(group, bindGroup.get(), offset.size(), offset.data());
         last_set_bind_group        = group;
         last_set_bind_group_offset = offset;
         last_set_bind_group_id     = bindGroup.id;
      }
   }

   template <typename T>
   void setVertexBuffer(Buffer<T>& buffer, uint32_t index) {
      if (last_set_vertex_buffer != (int32_t)buffer.summed_id() || last_set_vertex_buffer_index != (int32_t)index) {
         renderPass_.setVertexBuffer(index, buffer.get(), 0, buffer.sizeBytes());
         last_set_vertex_buffer       = buffer.summed_id();
         last_set_vertex_buffer_index = index;
      }
   }

   void setIndexBuffer(const IndexBuffer& buffer) {
      if (last_set_index_buffer != (int32_t)buffer.summed_id()) {
         renderPass_.setIndexBuffer(buffer.get(), wgpu::IndexFormat::Uint16, 0, buffer.sizeBytes());
         last_set_index_buffer = buffer.summed_id();
      }
   }

   template <typename Pipeline, typename... Vertices>
   void DrawInstanced(const Pipeline& pipeline, const IndexBuffer& indexBuffer, BindGroup bindGroup,
                      std::vector<uint32_t> offset, uint32_t instanceCount, Buffer<Vertices>&... bufs) {
      setPipeline(pipeline);
      setBindGroup(0, bindGroup, offset);

      size_t bufferIndex = 0;
      // Apply setBuffer to each vertex buffer
      (setVertexBuffer(bufs, bufferIndex++), ...);

      setIndexBuffer(indexBuffer);
      renderPass_.drawIndexed(indexBuffer.count(), instanceCount, 0, 0, 0);
   }

   template <typename Pipeline, typename Vertex>
   void Draw(const Pipeline& pipeline, Buffer<Vertex>& pointBuffer, const IndexBuffer& indexBuffer, BindGroup bindGroup,
             std::vector<uint32_t> offset) {
      DrawInstanced(pipeline, indexBuffer, bindGroup, offset, 1, pointBuffer);
   }

private:
   wgpu::RenderPassEncoder renderPass_;

   int32_t               last_set_render_pipeline = -1;
   int32_t               last_set_bind_group      = -1;
   int32_t               last_set_bind_group_id   = -1;
   std::vector<uint32_t> last_set_bind_group_offset;
   int32_t               last_set_vertex_buffer       = -1;
   int32_t               last_set_vertex_buffer_index = -1;
   int32_t               last_set_index_buffer        = -1;
};
