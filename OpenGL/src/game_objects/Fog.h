#pragma once
#include "../World.h"
#include "GameObject.h"
#include "clipper2/clipper.h"

class Fog : public GameObject {
public:
   Fog();
   virtual void render(Renderer& renderer, RenderPass& renderPass) override;
   virtual void update() override;

   glm::vec4 mainFogColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
   glm::vec4 tintFogColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);

private:
   virtual void renderPolyTree(Renderer& renderer, RenderPass& renderPass, const Clipper2Lib::PolyTreeD& polytree,
                               const UniformBufferView<FogFragmentUniform>& fragmentUniform) const;


private:
   UniformBuffer<FogVertexUniform>       vertexUniform;
   UniformBufferView<FogFragmentUniform> fragmentUniformWalls;
   UniformBufferView<FogFragmentUniform> fragmentUniformOther;
};
