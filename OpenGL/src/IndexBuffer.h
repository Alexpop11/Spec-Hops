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
   template <typename Container>
   IndexBuffer(const Container& data) {
      static_assert(std::is_same_v<typename Container::value_type, uint32_t>,
                    "Container must contain uint32_t elements");
      m_Count = data.size();
      GLCall(glGenBuffers(1, &m_RendererID));
      GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
      GLCall(glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(uint32_t), data.data(), GL_STATIC_DRAW));
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
