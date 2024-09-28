#pragma once
#include <utility> // for std::swap
#include "WeakMemoizeConstructor.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"

class IndexBuffer {
private:
   wrap_t<unsigned int> m_RendererID;
   wrap_t<unsigned int> m_Count;

public:
   template <size_t N>
   IndexBuffer(const std::array<unsigned int, N>& data) {
      m_Count = N;
      GLCall(glGenBuffers(1, &m_RendererID));
      GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
      GLCall(glBufferData(GL_ARRAY_BUFFER, N * sizeof(unsigned int), data.data(), GL_STATIC_DRAW));
   }

   IndexBuffer(const IndexBuffer&)             = delete;
   IndexBuffer(IndexBuffer&& other)            = default;
   IndexBuffer& operator=(const IndexBuffer&)  = delete;
   IndexBuffer& operator=(IndexBuffer&& other) = default;


   // Swap function
   friend void swap(IndexBuffer& first, IndexBuffer& second) noexcept {
      using std::swap;
      swap(first.m_RendererID, second.m_RendererID);
      swap(first.m_Count, second.m_Count);
   }

   ~IndexBuffer();

   void Bind() const;
   void Unbind() const;

   inline unsigned int GetCount() const { return m_Count; }

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(IndexBuffer)
};
