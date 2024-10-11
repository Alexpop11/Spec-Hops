#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"
#include "Texture.h"

class SquareObjectData {
   IndexBuffer                indexBuffer;
   Buffer<SquareObjectVertex> pointBuffer;
};

class Renderer {
public:
   Renderer();

   RenderPipeline<BindGroupLayouts<BindGroupLayout<StarUniformBinding>>, VertexBufferLayout<glm::vec2>> stars;
   RenderPipeline<BindGroupLayouts<SquareObjectLayout>, VertexBufferLayout<glm::vec2, glm::vec2>>       squareObject;
   Texture                                                                                              player;

   // SquareObjectData squareObjectData;

   template <typename B, typename V>
   void Draw(RenderPipeline<B, V>& pipeline) {};

   wgpu::RenderPassEncoder renderPass;
};
