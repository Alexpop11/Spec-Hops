#pragma once

#include <webgpu/webgpu.hpp>

#include "RenderPipeline.h"

class Renderer {
public:
   Renderer();

   RenderPipeline<BindGroupLayout<StarUniformBinding>, VertexBufferLayout<glm::vec2>> stars;
};
