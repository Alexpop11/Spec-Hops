// TextureSampler.hpp
#pragma once

#include <webgpu/webgpu.hpp>
#include <iostream>

class TextureSampler {
public:
   // Constructor: Creates a WebGPU sampler with the specified descriptor
   TextureSampler()
      : device_(Application::get().getDevice()) {
      wgpu::SamplerDescriptor samplerDesc = {};
      samplerDesc.minFilter               = wgpu::FilterMode::Nearest;
      samplerDesc.magFilter               = wgpu::FilterMode::Nearest;
      samplerDesc.mipmapFilter            = wgpu::MipmapFilterMode::Nearest;
      samplerDesc.addressModeU            = wgpu::AddressMode::ClampToEdge;
      samplerDesc.addressModeV            = wgpu::AddressMode::ClampToEdge;
      samplerDesc.addressModeW            = wgpu::AddressMode::ClampToEdge;
      samplerDesc.maxAnisotropy           = 1;

      sampler_ = device_.createSampler(samplerDesc);
      if (!sampler_) {
         std::cerr << "Failed to create sampler." << std::endl;
         // Handle sampler creation failure as needed
      }
   }

   // Destructor: Releases the sampler resource
   ~TextureSampler() {
      if (sampler_) {
         sampler_.release();
      }
   }

   // Deleted copy constructor and assignment operator to prevent copying
   TextureSampler(const TextureSampler&)            = delete;
   TextureSampler& operator=(const TextureSampler&) = delete;

   // Move constructor
   TextureSampler(TextureSampler&& other) noexcept
      : device_(other.device_)
      , sampler_(other.sampler_) {
      other.sampler_ = nullptr;
   }

   // Move assignment operator
   TextureSampler& operator=(TextureSampler&& other) noexcept {
      if (this != &other) {
         if (sampler_) {
            sampler_.release();
         }
         sampler_       = other.sampler_;
         other.sampler_ = nullptr;
      }
      return *this;
   }

   // Getter for the sampler
   wgpu::Sampler&       getSampler() { return sampler_; }
   const wgpu::Sampler& getSampler() const { return sampler_; }

private:
   wgpu::Device& device_;
   wgpu::Sampler sampler_ = nullptr;
};
