#pragma once

#include <webgpu/webgpu.hpp>
#include "Buffer.h"
#include "BindGroup.h"
#include "Texture.h"
#include "TextureSampler.h"

class CommandEncoder;

class RenderPass {
public:
   explicit RenderPass(CommandEncoder& encoder);
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
   void setVertexBuffer(const Buffer<T>& buffer) {
      if (last_set_vertex_buffer != (int32_t)buffer.summed_id()) {
         renderPass_.setVertexBuffer(0, buffer.get(), 0, buffer.sizeBytes());
         last_set_vertex_buffer = buffer.summed_id();
      }
   }

   void setIndexBuffer(const IndexBuffer& buffer) {
      if (last_set_index_buffer != (int32_t)buffer.summed_id()) {
         renderPass_.setIndexBuffer(buffer.get(), wgpu::IndexFormat::Uint16, 0, buffer.sizeBytes());
         last_set_index_buffer = buffer.summed_id();
      }
   }

   template <typename Pipeline, typename Vertex>
   void Draw(const Pipeline& pipeline, const Buffer<Vertex>& pointBuffer, const IndexBuffer& indexBuffer,
             BindGroup bindGroup, std::vector<uint32_t> offset) {
      setPipeline(pipeline);
      setBindGroup(0, bindGroup, offset);
      setVertexBuffer(pointBuffer);
      setIndexBuffer(indexBuffer);
      renderPass_.drawIndexed(indexBuffer.count(), 1, 0, 0, 0);
   }

private:
   wgpu::RenderPassEncoder renderPass_;
   wgpu::TextureView       targetView_;

   int32_t               last_set_render_pipeline = -1;
   int32_t               last_set_bind_group      = -1;
   int32_t               last_set_bind_group_id   = -1;
   std::vector<uint32_t> last_set_bind_group_offset;
   int32_t               last_set_vertex_buffer = -1;
   int32_t               last_set_index_buffer  = -1;
};
