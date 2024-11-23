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

   RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayouts<VertexBufferLayout<glm::vec2>>> stars;
   RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayouts<VertexBufferLayout<glm::vec2, glm::vec2>>> squareObject;
   RenderPipeline<BindGroupLayouts<LineLayout>, VertexBufferLayouts<VertexBufferLayout<LineVertex>>> line;
   RenderPipeline<BindGroupLayouts<FogLayout>, VertexBufferLayouts<VertexBufferLayout<FogVertex>>> fog;
   RenderPipeline<BindGroupLayouts<ParticleLayout>, VertexBufferLayouts<VertexBufferLayout<glm::vec2>>> particles;

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


glm::mat4 CalculateModel(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale);
glm::mat4 CalculateView();
glm::mat4 CalculateProjection();
glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale);
