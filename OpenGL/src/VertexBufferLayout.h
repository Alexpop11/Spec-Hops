#pragma once

#include <vector>
#include "Renderer.h"
#include <GL/glew.h>
#include <array>

struct VertexBufferElement {
   uint32_t      type;
   uint32_t      count;
   unsigned char normalized;

   static uint32_t GetSizeOfType(uint32_t type) {
      switch (type) {
      case GL_FLOAT:
         return 4;
      case GL_UNSIGNED_INT:
         return 4;
      case GL_UNSIGNED_BYTE:
         return 1;
      default:
         ASSERT(false);
      }
      return 0;
   }

   std::vector<uint32_t> data() const { return std::vector<uint32_t>({type, count, normalized}); }
};

class VertexBufferLayout {
private:
   std::vector<VertexBufferElement> m_Elements;
   uint32_t                         m_Stride;

public:
   VertexBufferLayout()
      : m_Stride(0) {}

   template <typename T>
   void Push(uint32_t count);

   inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
   inline uint32_t                               GetStride() const { return m_Stride; }

   std::vector<uint32_t> data() const {
      std::vector<uint32_t> data;
      data.push_back(m_Elements.size());
      for (const auto& element : m_Elements) {
         auto element_data = element.data();
         data.insert(data.end(), element_data.begin(), element_data.end());
      }
      data.push_back(m_Stride);
      return data;
   }
};
