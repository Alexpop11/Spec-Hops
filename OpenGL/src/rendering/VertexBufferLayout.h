
#include <webgpu/webgpu.hpp>
#include <vector>
#include <string>
#include "glm/glm.hpp"

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

template <typename... Types>
struct VertexBufferLayout {
   std::vector<wgpu::VertexAttribute> Attributes() {
      std::vector<wgpu::VertexAttribute> vertexAttribs;
      std::size_t                        offset = 0;
      std::size_t                        index  = 0;

      (vertexAttribs.push_back(CreateAttribute<Types>(index++, offset)), ...);

      return vertexAttribs;
   }

   std::tuple<wgpu::VertexBufferLayout, std::unique_ptr<std::vector<wgpu::VertexAttribute>>> CreateLayout() {
      auto vertexAttribs = std::make_unique<std::vector<wgpu::VertexAttribute>>(Attributes());

      wgpu::VertexBufferLayout layout;
      layout.arrayStride    = (type_size<Types>() + ...); // sum of the sizes of all types
      layout.stepMode       = wgpu::VertexStepMode::Vertex;
      layout.attributeCount = vertexAttribs->size();
      layout.attributes     = vertexAttribs->data();

      return std::make_tuple(layout, std::move(vertexAttribs));
   }

private:
   template <typename T>
   wgpu::VertexAttribute CreateAttribute(std::size_t shaderLocation, std::size_t& offset) {
      wgpu::VertexAttribute attribute;
      attribute.shaderLocation = shaderLocation;
      attribute.format         = to_vertex_format<T>();
      attribute.offset         = offset;
      offset += type_size<T>();
      return attribute;
   }
};
