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
   shader = Shader::create(Renderer::ResPath() + "shaders/fog.shader");
}

void Fog::setUpShader(Renderer& renderer) {
   GameObject::setUpShader(renderer);
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
      auto hull = hullRegion->Polygon();
      // std::vector<std::vector<PointD>> invisibility = {hull};
      // std::vector<uint32_t>            indices      = mapbox::earcut<uint32_t>(invisibility);

      // I can print this and see the verticies and indicies
      // std::cout << "Drawing fog" << std::endl;
      // std::cout << "verticies: " << hull << std::endl;
      // std::cout << "indicies: ";
      // for (auto index : indices) {
      // std::cout << index << " ";
      //}
      // std::cout << std::endl;

      // renderer.Draw(*va, *ib, *shader);
   }
}

void Fog::update() {}
