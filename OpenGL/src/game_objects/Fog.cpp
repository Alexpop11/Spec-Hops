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
   shader = Shader::create(Renderer::ResPath() + "Shaders/fog.shader");
}

void Fog::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
}

void Fog::render(Renderer& renderer) {
   GameObject::render(renderer);

   // Collect all tile bounds
   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::getAll<Tile>(); // Simplified retrieval of all tiles
   for (auto tile : tiles) {
      allBounds.push_back(tile->getBounds());
   }

   // Get the player
   auto player = World::getFirst<Player>(); // Simplified retrieval of the first player

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

   // Compute the visibility polygon and add it as the clip
   clipper.AddClip({ComputeVisibilityPolygon(player->position, flattened)});
   bool showAllWalls = true;
   if (showAllWalls) {
      clipper.AddClip({flattened});
   }

   // Compute the difference to get invisibility regions
   PolyTreeD invisibilityPaths;
   clipper.Execute(ClipType::Difference, FillRule::NonZero, invisibilityPaths);

   // Render the invisibility regions
   for (auto& hullRegion : invisibilityPaths) {
      std::vector<PointD>              hull         = hullRegion->Polygon();
      std::vector<std::vector<PointD>> invisibility = {hull};
      for (auto& visibleRegion : *hullRegion) {
         invisibility.push_back(visibleRegion->Polygon());
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

      if (va && ib && shader) {
         renderer.Draw(*va, *ib, *shader);
      }
   }
}

void Fog::update() {}
