#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::~VertexBuffer() {
   GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const {
   GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const {
   GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
