#include "Fog.h"
#include "Tile.h"
#include "Player.h"
#include <vector>
#include "clipper2/clipper.h"
#include "../GeometryUtils.h"
#include "earcut.hpp"

using namespace Clipper2Lib;
using namespace GeometryUtils;

Fog::Fog()
   : GameObject("Fog of War", DrawPriority::Fog, {0, 0})
   , vertexUniform(
        UniformBuffer<FogVertexUniform>({FogVertexUniform(MVP())},
                                        wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Uniform)))
   , fragmentUniformWalls(
        UniformBufferView<FogFragmentUniform>::create(FogFragmentUniform(mainFogColor, tintFogColor, {0, 0})))
   , fragmentUniformOther(
        UniformBufferView<FogFragmentUniform>::create(FogFragmentUniform(mainFogColor, mainFogColor, {0, 0}))) {}

void Fog::render(Renderer& renderer, RenderPass& renderPass) {
   bool showWalls = true;
   vertexUniform.upload({FogVertexUniform(MVP())});

   // Collect all tile bounds
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::getAll<Tile>(); // Simplified retrieval of all tiles
   for (auto tile : tiles) {
      allBounds.push_back(tile->getBounds());
   }

   // Get the player
   auto player = World::getFirst<Player>(); // Simplified retrieval of the first player
   fragmentUniformWalls.Update(FogFragmentUniform(mainFogColor, tintFogColor, player->position));
   fragmentUniformOther.Update(FogFragmentUniform(mainFogColor, mainFogColor, player->position));

   // Compute the union of all tile bounds
   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   auto flattened = FlattenPolyPathD(combined);

   // Prepare the hull for clipping
   ClipperD clipper;
   PathsD   hullPaths;
   for (auto& child : combined) {
      hullPaths.push_back(child->Polygon());
   }
   clipper.AddSubject(hullPaths);

   // Compute the visibility polygon
   auto visibility = ComputeVisibilityPolygon(player->position, flattened);

   // Compute the areas occluded
   clipper.AddClip({visibility});
   if (showWalls) {
      clipper.AddClip({flattened});
   }
   // Compute the difference to get invisibility regions
   PolyTreeD invisibilityPaths;
   clipper.Execute(ClipType::Difference, FillRule::NonZero, invisibilityPaths);

   // Render the invisibility regions
   renderPolyTree(renderer, renderPass, invisibilityPaths, fragmentUniformOther);

   if (showWalls) {
      // Tint all the walls that are not visible
      ClipperD tint;
      tint.AddSubject({flattened});
      tint.AddClip({visibility});
      PolyTreeD tintPaths;
      tint.Execute(ClipType::Difference, FillRule::NonZero, tintPaths);

      renderPolyTree(renderer, renderPass, tintPaths, fragmentUniformWalls);
   }
}

void Fog::update() {}

void Fog::renderPolyTree(Renderer& renderer, RenderPass& renderPass, const PolyTreeD& polytree,
                         const UniformBufferView<FogFragmentUniform>& fragmentUniform) const {
   for (auto& shadedRegion : polytree) {
      std::vector<PointD>              shaded       = shadedRegion->Polygon();
      std::vector<std::vector<PointD>> invisibility = {shaded};
      for (auto& holeRegion : *shadedRegion) {
         invisibility.push_back(holeRegion->Polygon());
         renderPolyTree(renderer, renderPass, *holeRegion, fragmentUniform);
      }

      // Triangulate the invisibility regions
      std::vector<uint16_t> indices = mapbox::earcut<uint16_t>(invisibility);

      // Collect vertices
      std::vector<FogVertex> vertices;
      for (const auto& shape : invisibility) {
         for (const auto& point : shape) {
            vertices.emplace_back(glm::vec2(point.x, point.y));
         }
      }

      // make vertex buffer
      auto vertexBuffer =
         Buffer<FogVertex>(vertices, wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Vertex));
      // make index buffer
      auto indexBuffer =
         IndexBuffer(indices, wgpu::bothBufferUsages(wgpu::BufferUsage::CopyDst, wgpu::BufferUsage::Index));
      // make bind group
      BindGroup bindGroup =
         FogLayout::ToBindGroup(renderer.device, std::forward_as_tuple(vertexUniform, 0), fragmentUniform);
      renderPass.Draw(renderer.fog, vertexBuffer, indexBuffer, bindGroup, {(unsigned int)fragmentUniform.getOffset()});
   }
}
