#pragma once

#include <webgpu/webgpu.hpp>
#include "Buffer.h"
#include "BindGroup.h"
#include "ComputePipeline.h"

class CommandEncoder;

class ComputePass {
public:
   explicit ComputePass(CommandEncoder& encoder, wgpu::TextureView& targetView);
   ~ComputePass();

   wgpu::ComputePassEncoder& get();

   template <typename T>
   void setPipeline(const T& pass) {
      if (last_set_compute_pipeline != pass.id) {
         computePass_.setPipeline(pass.GetPipeline());
         last_set_compute_pipeline = pass.id;
      }
   }

   void setBindGroup(uint32_t group, BindGroup bindGroup, std::vector<uint32_t> offset) {
      // TODO: this is overly conservative because we could be checking the that the bind group / offset is the same as
      // the last time we set that index
      if ((last_set_bind_group != (int32_t)group) ||           // Check index
          (bindGroup.id != (int32_t)last_set_bind_group_id) || // Check bind group id
          offset != last_set_bind_group_offset                 // check offset
      ) {
         computePass_.setBindGroup(group, bindGroup.get(), offset.size(), offset.data());
         last_set_bind_group        = group;
         last_set_bind_group_offset = offset;
         last_set_bind_group_id     = bindGroup.id;
      }
   }

   template <typename BGLs>
   void dispatch(ComputePipeline<BGLs>& pipeline, BindGroup bindGroup, uint32_t x, uint32_t y = 1, uint32_t z = 1) {
      setPipeline(pipeline);
      setBindGroup(0, bindGroup, {});
      computePass_.dispatchWorkgroups(x, y, z);
   }


private:
   wgpu::ComputePassEncoder computePass_;

   int32_t               last_set_compute_pipeline = -1;
   int32_t               last_set_bind_group       = -1;
   int32_t               last_set_bind_group_id    = -1;
   std::vector<uint32_t> last_set_bind_group_offset;
   int32_t               last_set_vertex_buffer = -1;
   int32_t               last_set_index_buffer  = -1;
};
