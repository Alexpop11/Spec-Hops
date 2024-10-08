#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <type_traits>

enum class BindingType {
   Buffer,
   Sampler,
   Texture,
};

// Helper struct for buffer bindings (e.g., Uniform or Storage buffers)
template <typename T, uint32_t BindingIndex, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType>
struct BufferBinding {
   using Type                                           = T;
   static constexpr uint32_t                binding     = BindingIndex;
   static constexpr wgpu::ShaderStage       visibility  = Visibility;
   static constexpr wgpu::BufferBindingType bufferType  = BufferType;
   static constexpr BindingType             bindingType = BindingType::Buffer;
};

// Helper struct for sampler bindings
template <uint32_t BindingIndex, wgpu::ShaderStage Visibility, wgpu::SamplerBindingType SamplerType>
struct SamplerBinding {
   static constexpr uint32_t                 binding     = BindingIndex;
   static constexpr wgpu::ShaderStage        visibility  = Visibility;
   static constexpr wgpu::SamplerBindingType samplerType = SamplerType;
   static constexpr BindingType              bindingType = BindingType::Sampler;
};

// Helper struct for texture bindings
template <uint32_t BindingIndex, wgpu::ShaderStage Visibility, wgpu::TextureSampleType SampleType,
          wgpu::TextureViewDimension ViewDimension, bool Multisampled = false>
struct TextureBinding {
   static constexpr uint32_t                   binding       = BindingIndex;
   static constexpr wgpu::ShaderStage          visibility    = Visibility;
   static constexpr wgpu::TextureSampleType    sampleType    = SampleType;
   static constexpr wgpu::TextureViewDimension viewDimension = ViewDimension;
   static constexpr bool                       multisampled  = Multisampled;
   static constexpr BindingType                bindingType   = BindingType::Texture;
};

// Helper template that will cause a compile-time error
template <typename T>
struct PrintType;


// Generator for BindGroupLayout
template <typename... Bindings>
struct BindGroupLayoutGenerator {
   static std::vector<wgpu::BindGroupLayoutEntry> GetEntries() {
      std::vector<wgpu::BindGroupLayoutEntry> entries;
      (entries.push_back(CreateEntry<Bindings>()), ...);
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
   static wgpu::BindGroupLayoutEntry CreateEntry() {
      wgpu::BindGroupLayoutEntry entry{};
      entry.binding    = Binding::binding;
      entry.visibility = Binding::visibility;

      if constexpr (Binding::bindingType == BindingType::Buffer) {
         // Buffer binding
         entry.buffer.type             = Binding::bufferType;
         entry.buffer.hasDynamicOffset = false;
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
