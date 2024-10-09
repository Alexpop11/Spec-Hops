#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"

class SquareObjectData {
   Buffer<uint16_t>           indexBuffer;
   Buffer<SquareObjectVertex> pointBuffer;
};

class Renderer {
public:
   Renderer();

   RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>> stars;
   RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>       squareObject;

   // SquareObjectData squareObjectData;

   template <typename B, typename V>
   void Draw(RenderPipeline<B, V>& pipeline) {};
};
