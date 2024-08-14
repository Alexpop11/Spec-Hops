#pragma once

class VertexBuffer {
private:
   unsigned int m_RendererID;

public:
   VertexBuffer(const void* data, unsigned int size);
   VertexBuffer(const VertexBuffer&)            = delete;
   VertexBuffer& operator=(const VertexBuffer&) = delete;
   ~VertexBuffer();


   void Bind() const;
   void Unbind() const;
};
