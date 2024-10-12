#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"
#include "Texture.h"
#include "TextureSampler.h"
#include "CommandEncoder.h"

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

   template <typename T>
   void setPipeline(const T& pass) {
      if (last_set_render_pipeline != pass.id) {
         renderPass.setPipeline(pass.GetPipeline());
         last_set_render_pipeline = pass.id;
      }
   }

   void setBindGroup(uint32_t index, BindGroup bindGroup, std::vector<uint32_t> offset) {
      // TODO: this is overly conservative because we could be checking the that the bind group / offset is the same as
      // the last time we set that index
      if ((last_set_bind_group_index != (int32_t)index) ||     // Check index
          (bindGroup.id != (int32_t)last_set_bind_group_id) || // Check bind group id
          offset != last_set_bind_group_offset                 // check offset
      ) {
         renderPass.setBindGroup(index, bindGroup.get(), offset.size(), offset.data());
         last_set_bind_group_index  = index;
         last_set_bind_group_offset = offset;
         last_set_bind_group_id     = bindGroup.id;
      }
   }

   // Debug assistance
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);
   void        DrawDebug();

   void FinishFrame() {
      CommandEncoder::DestroyDeadBuffers();
      last_set_render_pipeline   = -1;
      last_set_bind_group_index  = -1;
      last_set_bind_group_id     = -1;
      last_set_bind_group_offset = std::vector<uint32_t>();
   }

private:
   void DrawLine(Line line);

   int32_t               last_set_render_pipeline  = -1;
   int32_t               last_set_bind_group_index = -1;
   int32_t               last_set_bind_group_id    = -1;
   std::vector<uint32_t> last_set_bind_group_offset;
};


glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale);
