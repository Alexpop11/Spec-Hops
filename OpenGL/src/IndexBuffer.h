#pragma once
#include <utility> // for std::swap

class IndexBuffer {
private:
   unsigned int m_RendererID;
   unsigned int m_Count;

public:
   IndexBuffer(const unsigned int* data, unsigned int count);

   // Delete copy constructor
   IndexBuffer(const IndexBuffer&) = delete;

   // Move constructor
   IndexBuffer(IndexBuffer&& other) noexcept
      : m_RendererID(other.m_RendererID)
      , m_Count(other.m_Count) {
      other.m_RendererID = 0;
      other.m_Count      = 0;
   }

   // Delete copy assignment operator
   IndexBuffer& operator=(const IndexBuffer&) = delete;

   // Move assignment operator using swap
   IndexBuffer& operator=(IndexBuffer&& other) noexcept {
      swap(*this, other);
      return *this;
   }

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
};
