#pragma once

#include <webgpu/webgpu.hpp>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <cstring>     // For std::memcpy
#include <stb_image.h> // Ensure stb_image is included in your project

// Texture abstraction class
class Texture {
public:
   // Constructor: Loads an image from the given path and creates a WebGPU texture
   Texture(wgpu::Device& device, wgpu::Queue& queue, const std::string& path)
      : device_(device)
      , queue_(queue)
      , path_(path) {
      std::cout << "Initializing texture: " << path_ << std::endl;

      // Load image data using stb_image
      stbi_set_flip_vertically_on_load(1);                                      // Flip the image vertically if needed
      m_LocalBuffer = stbi_load(path_.c_str(), &m_Width, &m_Height, &m_BPP, 4); // Force RGBA

      if (!m_LocalBuffer) {
         std::cerr << "Failed to load image: " << path_ << std::endl;
         return;
      }

      // Calculate image size
      size_t imageSize = m_Width * m_Height * 4; // 4 bytes per pixel (RGBA8)

      // Define the texture descriptor
      wgpu::TextureDescriptor textureDesc = {};
      textureDesc.size.width              = m_Width;
      textureDesc.size.height             = m_Height;
      textureDesc.size.depthOrArrayLayers = 1;
      textureDesc.mipLevelCount           = 1;
      textureDesc.sampleCount             = 1;
      textureDesc.dimension               = wgpu::TextureDimension::_2D;
      textureDesc.format                  = wgpu::TextureFormat::RGBA8Unorm;
      textureDesc.usage =
         wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment;

      // Create the texture
      texture_ = device_.createTexture(textureDesc);
      if (!texture_) {
         std::cerr << "Failed to create texture." << std::endl;
         stbi_image_free(m_LocalBuffer);
         m_LocalBuffer = nullptr;
         return;
      }

      // Define the texture view descriptor
      wgpu::TextureViewDescriptor textureViewDesc = {};
      textureViewDesc.aspect                      = wgpu::TextureAspect::All;
      textureViewDesc.baseArrayLayer              = 0;
      textureViewDesc.arrayLayerCount             = 1;
      textureViewDesc.baseMipLevel                = 0;
      textureViewDesc.mipLevelCount               = 1;
      textureViewDesc.dimension                   = wgpu::TextureViewDimension::_2D;
      textureViewDesc.format                      = textureDesc.format;

      // Create the texture view
      textureView_ = texture_.createView(textureViewDesc);
      if (!textureView_) {
         std::cerr << "Failed to create texture view." << std::endl;
         texture_.destroy();
         texture_ = nullptr;
         stbi_image_free(m_LocalBuffer);
         m_LocalBuffer = nullptr;
         return;
      }

      // Prepare image data for upload
      std::vector<uint8_t> pixels(imageSize);
      std::memcpy(pixels.data(), m_LocalBuffer, imageSize);

      // Define the destination for the texture upload
      wgpu::ImageCopyTexture destination = {};
      destination.texture                = texture_;
      destination.mipLevel               = 0;
      destination.origin                 = {0, 0, 0}; // Equivalent of the offset argument of Queue::writeBuffer
      destination.aspect                 = wgpu::TextureAspect::All; // Only relevant for depth/Stencil textures

      // Define the data layout of the source image
      wgpu::TextureDataLayout dataLayout = {};
      dataLayout.offset                  = 0;
      dataLayout.bytesPerRow             = 4 * m_Width;
      dataLayout.rowsPerImage            = m_Height;

      // Define the size of the copy
      wgpu::Extent3D copySize     = {};
      copySize.width              = m_Width;
      copySize.height             = m_Height;
      copySize.depthOrArrayLayers = 1;

      // Upload the texture data
      queue.writeTexture(destination, pixels.data(), pixels.size(), dataLayout, copySize);

      // Create a sampler
      wgpu::SamplerDescriptor samplerDesc = {};
      samplerDesc.minFilter               = wgpu::FilterMode::Nearest;
      samplerDesc.magFilter               = wgpu::FilterMode::Nearest;
      samplerDesc.mipmapFilter            = wgpu::MipmapFilterMode::Nearest;
      samplerDesc.addressModeU            = wgpu::AddressMode::ClampToEdge;
      samplerDesc.addressModeV            = wgpu::AddressMode::ClampToEdge;
      samplerDesc.addressModeW            = wgpu::AddressMode::ClampToEdge;
      samplerDesc.maxAnisotropy           = 1;


      sampler_ = device_.createSampler(samplerDesc);

      // Free the local image data as it's no longer needed
      stbi_image_free(m_LocalBuffer);
      m_LocalBuffer = nullptr;
   }

   // Destructor: Releases the texture and associated resources
   ~Texture() {
      if (sampler_) {
         sampler_.release();
      }
      if (textureView_) {
         textureView_.release();
      }
      if (texture_) {
         texture_.release();
      }
      // No need to free m_LocalBuffer here as it's already freed in the constructor
   }

   // Deleted copy constructor and assignment operator to prevent copying
   Texture(const Texture&)            = delete;
   Texture& operator=(const Texture&) = delete;

   // Move constructor
   Texture(Texture&& other) noexcept
      : device_(other.device_)
      , queue_(other.queue_)
      , texture_(other.texture_)
      , textureView_(other.textureView_)
      , sampler_(other.sampler_)
      , path_(std::move(other.path_))
      , m_Width(other.m_Width)
      , m_Height(other.m_Height)
      , m_BPP(other.m_BPP)
      , m_LocalBuffer(other.m_LocalBuffer) {
      other.texture_      = nullptr;
      other.textureView_  = nullptr;
      other.sampler_      = nullptr;
      other.m_LocalBuffer = nullptr;
      other.m_Width       = 0;
      other.m_Height      = 0;
      other.m_BPP         = 0;
   }

   // Move assignment operator
   Texture& operator=(Texture&& other) noexcept {
      if (this != &other) {
         // Clean up existing resources
         if (sampler_)
            sampler_.release();
         if (textureView_)
            textureView_.release();
         if (texture_)
            texture_.destroy();

         // Move resources from other
         device_       = other.device_;
         queue_        = other.queue_;
         texture_      = other.texture_;
         textureView_  = other.textureView_;
         sampler_      = other.sampler_;
         path_         = std::move(other.path_);
         m_Width       = other.m_Width;
         m_Height      = other.m_Height;
         m_BPP         = other.m_BPP;
         m_LocalBuffer = other.m_LocalBuffer;

         // Nullify other's resources
         other.texture_      = nullptr;
         other.textureView_  = nullptr;
         other.sampler_      = nullptr;
         other.m_LocalBuffer = nullptr;
         other.m_Width       = 0;
         other.m_Height      = 0;
         other.m_BPP         = 0;
      }
      return *this;
   }

   // Getter for the texture
   wgpu::Texture&       getTexture() { return texture_; }
   const wgpu::Texture& getTexture() const { return texture_; }

   // Getter for the texture view
   wgpu::TextureView&       getTextureView() { return textureView_; }
   const wgpu::TextureView& getTextureView() const { return textureView_; }

   // Getter for the sampler
   wgpu::Sampler&       getSampler() { return sampler_; }
   const wgpu::Sampler& getSampler() const { return sampler_; }

   // Getter for texture dimensions
   uint32_t getWidth() const { return m_Width; }
   uint32_t getHeight() const { return m_Height; }
   int32_t  getBPP() const { return m_BPP; }

private:
   wgpu::Device&     device_;
   wgpu::Queue&      queue_;
   wgpu::Texture     texture_     = nullptr;
   wgpu::TextureView textureView_ = nullptr;
   wgpu::Sampler     sampler_     = nullptr;

   std::string    path_;
   int            m_Width       = 0;
   int            m_Height      = 0;
   int            m_BPP         = 0;       // Bits per pixel (channels)
   unsigned char* m_LocalBuffer = nullptr; // Pointer to image data loaded by stb_image
};
