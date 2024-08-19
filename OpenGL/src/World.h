#pragma once

#include <functional>
#include "game_objects/GameObject.h"

class World {
public:
   static std::vector<std::unique_ptr<GameObject>> gameobjects;
   static std::vector<std::unique_ptr<GameObject>> gameobjectstoadd;

   template <typename T>
   static std::vector<T*> where(std::function<bool(const T&)> condition) {
      std::vector<T*> filteredObjects;
      for (auto& gameobject : gameobjects) {
         T* castedObject = dynamic_cast<T*>(gameobject.get());
         if (castedObject && condition(*castedObject)) {
            filteredObjects.push_back(castedObject);
         }
      }
      return filteredObjects;
   }

   template <typename T>
   static std::vector<T*> at(float x, float y) {
      std::vector<T*> filteredObjects;
      return where<T>([&](const T& obj) { return obj.x == x && obj.y == y; });
   }

   static void LoadMap(const std::string& map_path);

   static void UpdateObjects();
   static void TickObjects();
};
