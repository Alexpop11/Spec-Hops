// World.h
#pragma once

#include <filesystem>
#include <functional>

#include "game_objects/GameObject.h"
#include "rendering/Renderer.h"

class World {
public:
   static float                                                                           timeSpeed;
   static bool                                                                            settingTimeSpeed;
   static std::vector<std::shared_ptr<GameObject>>                                        gameobjects;
   static std::vector<std::unique_ptr<GameObject>>                                        gameobjectstoadd;
   inline static std::unordered_map<glm::ivec2, std::vector<std::shared_ptr<GameObject>>> positions = {};


   static bool ticksPaused();

   static void add_gameobject_recursive(GameObject* parent, GameObject* obj, std::vector<GameObject*>& allGameObjects) {
      if (!obj) return;
      
      obj->parent = parent;  // Set parent (null for top-level objects)
      allGameObjects.push_back(obj);
      
      auto children = obj->children();
      for (auto& child : children) {
         if (child) {
            add_gameobject_recursive(obj, child, allGameObjects);
         }
      }
   }

   static std::vector<GameObject*> get_gameobjects() {
      // return a vector of all gameobjects and their gameobjects.children recursively
      std::vector<GameObject*> allGameObjects;
      for (auto& gameobject : gameobjects) {
         add_gameobject_recursive(nullptr, gameobject.get(), allGameObjects);
      }
      return allGameObjects;
   }

   template <typename T>
   static std::vector<std::shared_ptr<T>> where(std::function<bool(const T&)> condition) {
      std::vector<std::shared_ptr<T>> filteredObjects;
      for (auto& gameobject : gameobjects) {
         std::shared_ptr<T> castedObject = std::dynamic_pointer_cast<T>(gameobject);
         if (castedObject && condition(*castedObject)) {
            filteredObjects.push_back(castedObject);
         }
      }
      return filteredObjects;
   }

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
   static std::vector<std::shared_ptr<T>> at(int x, int y) {
      return where<T>([&](const T& obj) { return obj.getTile().x == x && obj.getTile().y == y; });
   }

   static void LoadMap(const std::filesystem::path& map_path);

   static void UpdateObjects();
   static void TickObjects();
   static void RenderObjects(Renderer& renderer, RenderPass& renderPass);
   static void ComputeObjects(Renderer& renderer, ComputePass& computePass);
   static bool shouldTick;
};
