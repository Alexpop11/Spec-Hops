#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include <tuple>
#include "glm/glm.hpp"

// Make a type like std::tuple<wgpu::VertexBufferLayout, std::vector<wgpu::VertexAttribute>>, but delete its copy and
// copy assignment operators and make its move constructor and move assignment operator default
struct VertexBufferInfo {
   wgpu::VertexBufferLayout                            layout;
   std::unique_ptr<std::vector<wgpu::VertexAttribute>> attributes;

   // add default constructor
   VertexBufferInfo() = default;

   VertexBufferInfo(const VertexBufferInfo&)            = delete;
   VertexBufferInfo& operator=(const VertexBufferInfo&) = delete;

   VertexBufferInfo(VertexBufferInfo&& other) noexcept            = default;
   VertexBufferInfo& operator=(VertexBufferInfo&& other) noexcept = default;
};

// Compose multiple vertex buffer layouts
template <typename... Layouts>
struct VertexBufferLayouts {
   static std::vector<VertexBufferInfo> CreateLayouts() {
      std::vector<VertexBufferInfo> layouts;
      layouts.reserve(sizeof...(Layouts));
      (layouts.push_back(Layouts::CreateLayout()), ...);

      // Each VBLayoutEntry must have a unique location
      // But each VBLayout as returned by CreateLayout starts at 0
      // So we need to renumber the locations
      std::size_t location = 0;
      for (auto& layout : layouts) {
         for (auto& attribute : *layout.attributes) {
            attribute.shaderLocation = location++;
         }
      }

      return layouts;
   }
};


// template function that converts a type to a wgpu::VertexFormat
template <typename T>
wgpu::VertexFormat to_vertex_format() {
   static_assert(std::is_same_v<T, float> || std::is_same_v<T, uint32_t> || std::is_same_v<T, glm::vec2> ||
                    std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>,
                 "Unsupported type for vertex format");
   if constexpr (std::is_same_v<T, float>) {
      return wgpu::VertexFormat::Float32;
   } else if constexpr (std::is_same_v<T, uint32_t>) {
      return wgpu::VertexFormat::Uint32;
   } else if constexpr (std::is_same_v<T, glm::vec2>) {
      return wgpu::VertexFormat::Float32x2;
   } else if constexpr (std::is_same_v<T, glm::vec3>) {
      return wgpu::VertexFormat::Float32x3;
   } else if constexpr (std::is_same_v<T, glm::vec4>) {
      return wgpu::VertexFormat::Float32x4;
   }
}

template <typename T>
constexpr std::size_t type_size() {
   return sizeof(T);
}

// Single vertex buffer layout
template <typename... Types>
struct VertexBufferLayout {
   static std::vector<wgpu::VertexAttribute> Attributes() {
      std::vector<wgpu::VertexAttribute> vertexAttribs;
      std::size_t                        offset = 0;
      std::size_t                        index  = 0;

      (vertexAttribs.push_back(CreateAttribute<Types>(index++, offset)), ...);

      return vertexAttribs;
   }

   static VertexBufferInfo CreateLayout() {
      VertexBufferInfo info;
      info.attributes            = std::make_unique<std::vector<wgpu::VertexAttribute>>(Attributes());
      info.layout.arrayStride    = (type_size<Types>() + ...); // sum of the sizes of all types
      info.layout.stepMode       = wgpu::VertexStepMode::Vertex;
      info.layout.attributeCount = info.attributes->size();
      info.layout.attributes     = info.attributes->data();

      return info;
   }

private:
   template <typename T>
   static wgpu::VertexAttribute CreateAttribute(std::size_t shaderLocation, std::size_t& offset) {
      wgpu::VertexAttribute attribute;
      attribute.shaderLocation = shaderLocation;
      attribute.format         = to_vertex_format<T>();
      attribute.offset         = offset;
      offset += type_size<T>();
      return attribute;
   }
};


template <typename... Types>
struct InstanceBufferLayout {
   static VertexBufferInfo CreateLayout() {
      auto info            = VertexBufferLayout<Types...>::CreateLayout();
      info.layout.stepMode = wgpu::VertexStepMode::Instance;
      return info;
   }
};
