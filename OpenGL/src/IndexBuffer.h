#pragma once
#include <utility> // for std::swap
#include "WeakMemoizeConstructor.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"

class IndexBuffer {
private:
   wrap_t<uint32_t> m_RendererID;
   wrap_t<uint32_t> m_Count;

public:
   template <size_t N>
   IndexBuffer(const std::array<uint32_t, N>& data) {
      m_Count = N;
      GLCall(glGenBuffers(1, &m_RendererID));
      GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
      GLCall(glBufferData(GL_ARRAY_BUFFER, N * sizeof(uint32_t), data.data(), GL_STATIC_DRAW));
   }

   IndexBuffer(const IndexBuffer&)             = delete;
   IndexBuffer(IndexBuffer&& other)            = default;
   IndexBuffer& operator=(const IndexBuffer&)  = delete;
   IndexBuffer& operator=(IndexBuffer&& other) = default;

   ~IndexBuffer();

   void Bind() const;
   void Unbind() const;

   inline uint32_t GetCount() const { return m_Count; }

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(IndexBuffer)
};
