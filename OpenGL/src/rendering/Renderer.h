#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"
#include "Texture.h"

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



   template <typename B, typename V>
   void Draw(RenderPipeline<B, V>& pipeline) {};

   wgpu::RenderPassEncoder renderPass;
   wgpu::Device            device;

   // Debug assistance
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
   static void DebugLine(glm::vec2 start, glm::vec2 end, glm::vec4 color);
   void        DrawDebug();

private:
   void DrawLine(Line line);
};


glm::mat4 CalculateMVP(const glm::vec2& objectPosition, float objectRotationDegrees, float objectScale);
