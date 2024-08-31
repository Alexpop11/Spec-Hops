#pragma once
#include <utility> // for std::swap

class VertexBuffer {
private:
   unsigned int m_RendererID;

public:
   VertexBuffer(const void* data, unsigned int size);

   // Delete copy constructor
   VertexBuffer(const VertexBuffer&) = delete;

   // Move constructor
   VertexBuffer(VertexBuffer&& other) noexcept
      : m_RendererID(other.m_RendererID) {
      other.m_RendererID = 0;
   }

   // Delete copy assignment operator
   VertexBuffer& operator=(const VertexBuffer&) = delete;

   // Move assignment operator using swap
   VertexBuffer& operator=(VertexBuffer&& other) noexcept {
      swap(*this, other);
      return *this;
   }

   // Swap function
   friend void swap(VertexBuffer& first, VertexBuffer& second) noexcept {
      using std::swap;
      swap(first.m_RendererID, second.m_RendererID);
   }

   ~VertexBuffer();

   void Bind() const;
   void Unbind() const;
};
