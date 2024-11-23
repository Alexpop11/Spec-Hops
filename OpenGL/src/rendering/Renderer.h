#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"
#include "RenderPass.h"
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
   RenderPipeline<BindGroupLayouts<FogLayout>, VertexBufferLayout<FogVertex>>                           fog;
   RenderPipeline<BindGroupLayouts<ParticleLayout>, VertexBufferLayout<glm::vec2>> particles;

   TextureSampler sampler;
   
   wgpu::Device            device;

   static glm::vec2 MousePos();
   static glm::vec2 ScreenToWorldPosition(const glm::vec2& screenPos);

   // Debug assistance
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);
   void        DrawDebug(RenderPass& renderPass);

   void FinishFrame() { CommandEncoder::DestroyDeadBuffers(); }

private:
   void DrawLine(Line line, RenderPass& renderPass);

   Buffer<LineVertex> linePoints;
   IndexBuffer        lineIndices;
};


glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale);
