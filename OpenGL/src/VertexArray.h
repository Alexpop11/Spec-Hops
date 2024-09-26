#pragma once
#include "VertexBuffer.h"
#include <utility> // for std::swap
#include "WeakMemoizeConstructor.h"
#include <vector>
#include <tuple>
#include <array>
#include <array>

class VertexBufferLayout;

class VertexArray {
private:
   unsigned int                               m_RendererID;
   std::vector<std::shared_ptr<VertexBuffer>> vb;

public:
   VertexArray(std::shared_ptr<VertexBuffer> vb, const VertexBufferLayout& layout);


   template <typename T, size_t N>
   VertexArray(const std::array<T, N>& vbData, const VertexBufferLayout& layout) {
      auto vbp = VertexBuffer::create(vbData);
      vb.push_back(vbp);
      VertexArray(vbp, layout);
   }


   VertexArray(const VertexArray&)             = delete;
   VertexArray(VertexArray&& other)            = default;
   VertexArray& operator=(const VertexArray&)  = delete;
   VertexArray& operator=(VertexArray&& other) = default;

   ~VertexArray();

   void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

   void Bind() const;
   void Unbind() const;

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(VertexArray)
};
