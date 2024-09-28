#pragma once

#include "Renderer.h"

class Texture {
private:
   wrap_t<unsigned int> m_RendererID;
   std::string          m_FilePath;
   unsigned char*       m_LocalBuffer;
   int                  m_Width, m_Height, m_BPP;

public:
   Texture(const std::string& path);
   ~Texture();

   void Bind(unsigned int slot = 0) const;
   void Unbind() const;

   inline int GetWidth() const { return m_Width; }
   inline int GetHeight() const { return m_Height; }

   Texture(const Texture&)             = delete;
   Texture(Texture&& other)            = default;
   Texture& operator=(const Texture&)  = delete;
   Texture& operator=(Texture&& other) = default;


   // Declare the global memoized constructor
   DECLARE_GLOBAL_MEMOIZED_CONSTRUCTOR(Texture)
};
