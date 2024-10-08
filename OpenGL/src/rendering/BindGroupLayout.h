#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <type_traits>
#include "Buffer.h"

enum class BindingType {
   Buffer,
   Sampler,
   Texture,
};

template <typename Binding>
struct GetWGPUType;

// Helper struct for buffer bindings (e.g., Uniform or Storage buffers)
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType, bool DynamicOffset = false>
struct BufferBinding {
   using Type                                             = T;
   static constexpr wgpu::ShaderStage       visibility    = Visibility;
   static constexpr wgpu::BufferBindingType bufferType    = BufferType;
   static constexpr BindingType             bindingType   = BindingType::Buffer;
   static constexpr bool                    dynamicOffset = DynamicOffset;
};
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType>
struct GetWGPUType<BufferBinding<T, Visibility, BufferType, false>> {
   using type = std::tuple<Buffer<T, BufferType == wgpu::BufferBindingType::Uniform>&, size_t>;
};
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType>
struct GetWGPUType<BufferBinding<T, Visibility, BufferType, true>> {
   using type = Buffer<T, BufferType == wgpu::BufferBindingType::Uniform>&;
};



// Helper struct for sampler bindings
template <wgpu::ShaderStage Visibility, wgpu::SamplerBindingType SamplerType>
struct SamplerBinding {
   static constexpr wgpu::ShaderStage        visibility  = Visibility;
   static constexpr wgpu::SamplerBindingType samplerType = SamplerType;
   static constexpr BindingType              bindingType = BindingType::Sampler;
};
template <wgpu::ShaderStage Visibility, wgpu::SamplerBindingType SamplerType>
struct GetWGPUType<SamplerBinding<Visibility, SamplerType>> {
   using type = wgpu::Sampler;
};

// Helper struct for texture bindings
template <wgpu::ShaderStage Visibility, wgpu::TextureSampleType SampleType, wgpu::TextureViewDimension ViewDimension,
          bool Multisampled = false>
struct TextureBinding {
   static constexpr wgpu::ShaderStage          visibility    = Visibility;
   static constexpr wgpu::TextureSampleType    sampleType    = SampleType;
   static constexpr wgpu::TextureViewDimension viewDimension = ViewDimension;
   static constexpr bool                       multisampled  = Multisampled;
   static constexpr BindingType                bindingType   = BindingType::Texture;
};
template <wgpu::ShaderStage Visibility, wgpu::TextureSampleType SampleType, wgpu::TextureViewDimension ViewDimension,
          bool Multisampled>
struct GetWGPUType<TextureBinding<Visibility, SampleType, ViewDimension, Multisampled>> {
   using type = wgpu::Texture;
};

template <typename Binding>
using WGPUType = typename GetWGPUType<Binding>::type;
template <typename T>
concept BindingC = requires { typename WGPUType<T>; };



// Generator for BindGroupLayout
template <typename... Bindings>
struct BindGroupLayout {
   static std::vector<wgpu::BindGroupLayoutEntry> GetEntries() {
      std::vector<wgpu::BindGroupLayoutEntry> entries;
      size_t                                  index = 0;
      (entries.push_back(CreateEntry<Bindings>(index)), ...);
      return entries;
   }

   static wgpu::BindGroupLayout CreateLayout(wgpu::Device& device) {
      auto                            entries = GetEntries();
      wgpu::BindGroupLayoutDescriptor descriptor{};
      descriptor.entryCount = static_cast<uint32_t>(entries.size());
      descriptor.entries    = entries.data();
      return device.createBindGroupLayout(descriptor);
   }


private:
   template <typename Binding>
   static wgpu::BindGroupLayoutEntry CreateEntry(size_t& index) {
      wgpu::BindGroupLayoutEntry entry{};
      entry.binding = index;
      index += 1;
      entry.visibility = Binding::visibility;

      if constexpr (Binding::bindingType == BindingType::Buffer) {
         // Buffer binding
         entry.buffer.type             = Binding::bufferType;
         entry.buffer.hasDynamicOffset = Binding::dynamicOffset;
         entry.buffer.minBindingSize   = sizeof(typename Binding::Type);
      } else if constexpr (Binding::bindingType == BindingType::Sampler) {
         // Sampler binding
         entry.sampler.type = Binding::samplerType;
      } else if constexpr (Binding::bindingType == BindingType::Texture) {
         // Texture binding
         entry.texture.sampleType    = Binding::sampleType;
         entry.texture.viewDimension = Binding::viewDimension;
         entry.texture.multisampled  = Binding::multisampled;
      } else {
         static_assert(false, "Unsupported binding type");
      }


      return entry;
   }
};

namespace wgpu {
constexpr wgpu::ShaderStage both(wgpu::ShaderStage lhs, wgpu::ShaderStage rhs) {
   return wgpu::ShaderStage(lhs.m_raw | rhs.m_raw);
}
} // namespace wgpu
