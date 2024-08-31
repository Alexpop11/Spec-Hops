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
   unsigned int m_RendererID;

public:
   VertexArray();

   VertexArray(const VertexBuffer& vb, const VertexBufferLayout& layout);

   // Delete copy constructor
   VertexArray(const VertexArray&) = delete;

   // Move constructor
   VertexArray(VertexArray&& other) noexcept
      : m_RendererID(other.m_RendererID) {
      other.m_RendererID = 0;
   }

   // Delete copy assignment operator
   VertexArray& operator=(const VertexArray&) = delete;

   // Move assignment operator using swap
   VertexArray& operator=(VertexArray&& other) noexcept {
      swap(*this, other);
      return *this;
   }

   // Swap function
   friend void swap(VertexArray& first, VertexArray& second) noexcept {
      using std::swap;
      swap(first.m_RendererID, second.m_RendererID);
   }

   ~VertexArray();

   void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

   void Bind() const;
   void Unbind() const;

   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(VertexArray)
};
