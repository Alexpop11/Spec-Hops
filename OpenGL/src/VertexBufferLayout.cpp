#include "VertexBufferLayout.h"

template <typename T>
void Push(uint32_t count) {
   // static_assert(false);
}

template <>
void VertexBufferLayout::Push<float>(uint32_t count) {
   m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
   m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template <>
void VertexBufferLayout::Push<uint32_t>(uint32_t count) {
   m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
   m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}


template <>
void VertexBufferLayout::Push<unsigned char>(uint32_t count) {
   m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
   m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
