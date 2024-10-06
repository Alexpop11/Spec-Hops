// World.h
#pragma once

#include <functional>
#include "game_objects/GameObject.h"
#include "Renderer.h"

class World {
public:
   static float                                    timeSpeed;
   static bool                                     settingTimeSpeed;
   static std::vector<std::unique_ptr<GameObject>> gameobjects;
   static std::vector<std::unique_ptr<GameObject>> gameobjectstoadd;
   static std::vector<GameObject*>                 get_gameobjects() {
      // return a vector of all gameobjects and their gameobjects.children
      std::vector<GameObject*> allGameObjects;
      for (auto& gameobject : gameobjects) {
         allGameObjects.push_back(gameobject.get());
         auto children = gameobject->children();
         allGameObjects.insert(allGameObjects.end(), children.begin(), children.end());
      }
      return allGameObjects;
   }

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

   // Implemented getAll<T>()
   template <typename T>
   static std::vector<T*> getAll() {
      std::vector<T*> allObjects;
      for (auto& gameobject : gameobjects) {
         T* castedObject = dynamic_cast<T*>(gameobject.get());
         if (castedObject) {
            allObjects.push_back(castedObject);
         }
      }
      return allObjects;
   }

   // Implemented getFirst<T>()
   template <typename T>
   static T* getFirst() {
      for (auto& gameobject : gameobjects) {
         T* castedObject = dynamic_cast<T*>(gameobject.get());
         if (castedObject) {
            return castedObject;
         }
      }
      return nullptr;
   }

   template <typename T>
   static std::vector<T*> at(int x, int y) {
      return where<T>([&](const T& obj) { return obj.tile_x == x && obj.tile_y == y; });
   }

   static void LoadMap(const std::string& map_path);

   static void UpdateObjects();
   static void TickObjects();
   static void RenderObjects(Renderer& renderer);
   static bool shouldTick;
};
