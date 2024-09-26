#pragma once
#include <utility> // for std::swap
#include "WeakMemoizeConstructor.h"
#include "Utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class VertexBuffer {
private:
   wrap_t<unsigned int> m_RendererID;

public:
   template <typename T, size_t N>
   VertexBuffer(const std::array<T, N>& data) {
      static_assert(std::is_trivially_copyable_v<T>, "Data must be trivially copyable");

      GLCall(glGenBuffers(1, &m_RendererID));
      GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
      GLCall(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW));
   }

   VertexBuffer(const VertexBuffer&)             = delete;
   VertexBuffer(VertexBuffer&& other)            = default;
   VertexBuffer& operator=(const VertexBuffer&)  = delete;
   VertexBuffer& operator=(VertexBuffer&& other) = default;

   ~VertexBuffer();

   void Bind() const;
   void Unbind() const;

   unsigned int GetRendererID() const { return m_RendererID; }

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(VertexBuffer)
};

namespace std {
template <>
struct hash<VertexBuffer> {
   std::size_t operator()(const VertexBuffer& vb) const { return std::hash<unsigned int>{}(vb.GetRendererID()); }
};
} // namespace std
