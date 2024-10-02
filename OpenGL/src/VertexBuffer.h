#pragma once
#include <utility>
#include "WeakMemoizeConstructor.hpp"
#include "Utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <type_traits>

class VertexBuffer {
private:
   wrap_t<uint32_t> m_RendererID;

public:
   template <typename Container>
   VertexBuffer(const Container& data) {
      using T = typename Container::value_type;
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

   uint32_t GetRendererID() const { return m_RendererID; }

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(VertexBuffer)
};
