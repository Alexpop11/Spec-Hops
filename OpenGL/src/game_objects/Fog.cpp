#include "Fog.h"
#include "Tile.h"
#include "Player.h"
#include <vector>
#include "clipper2/clipper.h"
#include "GeometryUtils.h"
#include "earcut.hpp"

using namespace Clipper2Lib;
using namespace GeometryUtils;

Fog::Fog()
   : GameObject("Fog of War", DrawPriority::Fog, {0, 0}) {
   shader       = Shader::create(Renderer::ResPath() + "Shaders/fog.shader");
   mainFogColor = {0.1, 0.1, 0.1, 1};
   tintFogColor = {0.1, 0.1, 0.1, 0};
}

void Fog::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
}

void Fog::render(Renderer& renderer) {
   
   GameObject::render(renderer);
   bool showWalls = true;

   // Collect all tile bounds
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::getAll<Tile>(); // Simplified retrieval of all tiles
   for (auto tile : tiles) {
      allBounds.push_back(tile->getBounds());
   }

   // Get the player
   auto player = World::getFirst<Player>(); // Simplified retrieval of the first player
   shader->SetUniform2f("uPlayerPosition", player->position);

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
   renderPolyTree(renderer, invisibilityPaths, mainFogColor, mainFogColor);

   if (showWalls) {
      // Tint all the walls that are not visible
      ClipperD tint;
      tint.AddSubject({flattened});
      tint.AddClip({visibility});
      PolyTreeD tintPaths;
      tint.Execute(ClipType::Difference, FillRule::NonZero, tintPaths);

      renderPolyTree(renderer, tintPaths, mainFogColor, tintFogColor);
   }
}

void Fog::update() {}

void Fog::renderPolyTree(Renderer& renderer, const PolyTreeD& polytree, glm::vec4 color, glm::vec4 bandColor) const {
   for (auto& shadedRegion : polytree) {
      std::vector<PointD>              shaded       = shadedRegion->Polygon();
      std::vector<std::vector<PointD>> invisibility = {shaded};
      for (auto& holeRegion : *shadedRegion) {
         invisibility.push_back(holeRegion->Polygon());
         renderPolyTree(renderer, *holeRegion, color, bandColor);
      }

      // Triangulate the invisibility regions
      std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(invisibility);

      // Collect vertices
      std::vector<glm::vec2> vertices;
      for (const auto& shape : invisibility) {
         for (const auto& point : shape) {
            vertices.emplace_back(point.x, point.y);
         }
      }

      // Create buffers and draw
      VertexBufferLayout layout;
      layout.Push<float>(2);
      auto vb = std::make_shared<VertexBuffer>(vertices);
      auto va = std::make_shared<VertexArray>(vb, layout);
      auto ib = std::make_shared<IndexBuffer>(indices);

      shader->SetUniform4f("u_Color", color);
      shader->SetUniform4f("u_BandColor", bandColor);

      if (va && ib && shader) {
         renderer.Draw(*va, *ib, *shader);
      }
   }
}
