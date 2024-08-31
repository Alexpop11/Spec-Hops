#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::~IndexBuffer() {
   GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const {
   GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const {
   GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
