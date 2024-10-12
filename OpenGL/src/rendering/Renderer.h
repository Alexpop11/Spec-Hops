#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"
#include "Texture.h"
#include "TextureSampler.h"
#include "CommandEncoder.h"
#include "Buffer.h"

#include "imgui.h"


class Line {
public:
   glm::vec2 start;
   glm::vec2 end;
   glm::vec4 color;
};

class Renderer {
public:
   Renderer();

   RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>> stars;
   RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>       squareObject;
   RenderPipeline<BindGroupLayouts<LineLayout>, VertexBufferLayout<LineVertex>>                         line;

   TextureSampler sampler;

   template <typename B, typename V>
   void Draw(RenderPipeline<B, V>& pipeline) {};

   wgpu::RenderPassEncoder renderPass;
   wgpu::Device            device;

   static glm::vec2 MousePos();
   static glm::vec2 ScreenToWorldPosition(const glm::vec2& screenPos);


   template <typename T>
   void setPipeline(const T& pass) {
      if (last_set_render_pipeline != pass.id) {
         renderPass.setPipeline(pass.GetPipeline());
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
         renderPass.setBindGroup(group, bindGroup.get(), offset.size(), offset.data());
         last_set_bind_group        = group;
         last_set_bind_group_offset = offset;
         last_set_bind_group_id     = bindGroup.id;
      }
   }

   template <typename T>
   void setVertexBuffer(const Buffer<T>& buffer) {
      if (last_set_vertex_buffer != (int32_t)buffer.summed_id()) {
         renderPass.setVertexBuffer(0, buffer.get(), 0, buffer.sizeBytes());
         last_set_vertex_buffer = buffer.summed_id();
      }
   }

   void setIndexBuffer(const IndexBuffer& buffer) {
      if (last_set_index_buffer != (int32_t)buffer.summed_id()) {
         renderPass.setIndexBuffer(buffer.get(), wgpu::IndexFormat::Uint16, 0, buffer.sizeBytes());
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
      renderPass.drawIndexed(indexBuffer.count(), 1, 0, 0, 0);
   }

   // Debug assistance
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);
   void        DrawDebug();

   void FinishFrame() {
      CommandEncoder::DestroyDeadBuffers();
      last_set_render_pipeline   = -1;
      last_set_bind_group        = -1;
      last_set_bind_group_id     = -1;
      last_set_bind_group_offset = std::vector<uint32_t>();
      last_set_vertex_buffer     = -1;
      last_set_index_buffer      = -1;
   }

private:
   void DrawLine(Line line);

   Buffer<LineVertex> linePoints;
   IndexBuffer        lineIndices;

   int32_t               last_set_render_pipeline = -1;
   int32_t               last_set_bind_group      = -1;
   int32_t               last_set_bind_group_id   = -1;
   std::vector<uint32_t> last_set_bind_group_offset;
   int32_t               last_set_vertex_buffer = -1;
   int32_t               last_set_index_buffer  = -1;
};


glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale);
