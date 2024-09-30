#include "Fog.h"
#include <array>
#include "Tile.h"
#include "Player.h"
#include <ranges>
#include <vector>
#include "clipper2/clipper.h"
#include "GeometryUtils.h"
#include "earcut.hpp"

using namespace Clipper2Lib;
using namespace GeometryUtils;

Fog::Fog()
   : GameObject("Fog of War", 6, {0, 0}) {
   // shader = Shader::create(Renderer::ResPath() + "shaders/fog.shader");

   texture = Texture::create(Renderer::ResPath() + "Textures/alternate-player.png");
   shader  = Shader::create(Renderer::ResPath() + "Shaders/fog.shader");

   std::array<float, 16> positions = {
      -0.5f, -0.5f, 0.0f, 0.0f, // 0
      0.5f,  -0.5f, 1.0f, 0.0f, // 1
      0.5f,  0.5f,  1.0f, 1.0f, // 2
      -0.5f, 0.5f,  0.0f, 1.0f  // 3
   };

   std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

   vb = VertexBuffer::create(positions);
   VertexBufferLayout layout;
   layout.Push<float>(2);
   layout.Push<float>(2);
   va = std::make_shared<VertexArray>(vb, layout);
   ib = IndexBuffer::create(indices);
}

void Fog::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
   texture->Bind();
   shader->SetUniform1i("u_Texture", 0);
   shader->SetUniform4f("u_Color", glm::vec4{0, 0, 0, 0});
}

void Fog::render(Renderer& renderer) {
   GameObject::render(renderer);

   std::vector<std::vector<glm::vec2>> allBounds;
   auto                                tiles = World::where<Tile>([&](const Tile& tile) { return true; });
   for (auto tile : tiles) {
      auto bounds = tile->getBounds();
      allBounds.push_back(bounds);
   }

   auto player = World::where<Player>([&](const Player& player) { return true; })[0];


   PolyTreeD combined;
   findPolygonUnion(allBounds, combined);
   auto flattened = FlattenPolyPathD(combined);

   ClipperD hull;
   PathsD   hullPaths;
   for (auto& child : combined) {
      hullPaths.push_back(child->Polygon());
   }
   hull.AddSubject(hullPaths);

   hull.AddClip({ComputeVisibilityPolygon(player->position, flattened)});

   PolyTreeD invisibilityPaths;
   auto      difference = hull.Execute(ClipType::Difference, FillRule::NonZero, invisibilityPaths);

   for (auto& hullRegion : invisibilityPaths) {
      std::vector<PointD>              hull         = hullRegion->Polygon();
      std::vector<std::vector<PointD>> invisibility = {hull};
      for (auto& visibleRegion : *hullRegion) {
         invisibility.push_back(visibleRegion->Polygon());
      }
      std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(invisibility);

      std::vector<glm::vec2> verticies;
      for (auto& shape : invisibility) {
         for (auto& point : shape) {
            verticies.push_back(glm::vec2{point.x, point.y});
         }
      }
      vb = std::make_shared<VertexBuffer>(verticies);
      VertexBufferLayout layout;
      layout.Push<float>(2);
      va = std::make_shared<VertexArray>(vb, layout);
      ib = std::make_shared<IndexBuffer>(indices);


      // draw if va, ib, and shader are set:
      if (va && ib && shader) {
         renderer.Draw(*va, *ib, *shader);
      }
      Renderer::DebugLine(position, position + glm::vec2{1, 1}, glm::vec4{1, 0, 0, 1});
   }
}

void Fog::update() {}
