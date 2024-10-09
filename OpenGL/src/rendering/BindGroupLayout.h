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
struct GetToBind;

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
struct GetToBind<BufferBinding<T, Visibility, BufferType, false>> {
   using type = std::tuple<Buffer<T, BufferType == wgpu::BufferBindingType::Uniform>&, size_t>;
};
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType>
struct GetToBind<BufferBinding<T, Visibility, BufferType, true>> {
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
struct GetToBind<SamplerBinding<Visibility, SamplerType>> {
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
struct GetToBind<TextureBinding<Visibility, SampleType, ViewDimension, Multisampled>> {
   using type = wgpu::Texture;
};

template <typename Binding>
using ToBind = typename GetToBind<Binding>::type;
template <typename T>
concept BindingC = requires { typename ToBind<T>; };



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

   // Conversion to bind group
   // ========================
   static wgpu::BindGroup BindGroup(wgpu::Device& device, ToBind<Bindings>&... resources) {
      // Create a tuple of references to the resources
      auto resourcesTuple = std::forward_as_tuple(resources...);

      // Create the bind group using the resources
      wgpu::BindGroup bindGroup = createBindGroup(device, resourcesTuple, std::index_sequence_for<Bindings...>{});

      return bindGroup;
   }

   template <typename Tuple, size_t... I>
   static wgpu::BindGroup createBindGroup(wgpu::Device& device, Tuple& resources, std::index_sequence<I...>) {
      // Create an array of BindGroupEntry
      std::array<wgpu::BindGroupEntry, sizeof...(Bindings)> entries = {
         createBindGroupEntry<I, Bindings>(device, std::get<I>(resources))...};

      // Create the BindGroupDescriptor
      wgpu::BindGroupDescriptor bindGroupDesc{};
      bindGroupDesc.layout     = CreateLayout(device);
      bindGroupDesc.entryCount = static_cast<uint32_t>(entries.size());
      bindGroupDesc.entries    = entries.data();

      // Create and return the BindGroup
      return device.createBindGroup(bindGroupDesc);
   }

   template <size_t I, typename Binding, typename Resource>
   static wgpu::BindGroupEntry createBindGroupEntry(wgpu::Device& device, Resource& resource) {
      wgpu::BindGroupEntry entry{};
      entry.binding = static_cast<uint32_t>(I);
      if constexpr (Binding::bindingType == BindingType::Buffer && !Binding::dynamicOffset) {
         // Assuming ToBind<T> is Buffer
         entry.buffer = std::get<0>(resource).get();
         entry.offset = std::get<1>(resource);
         entry.size   = sizeof(typename Binding::Type);
      } else if constexpr (Binding::bindingType == BindingType::Buffer && Binding::dynamicOffset) {
         entry.buffer = resource.get();
         entry.offset = 0;
         entry.size   = sizeof(typename Binding::Type);

      } else if constexpr (Binding::bindingType == BindingType::Sampler) {
         // Assuming ToBind<T> is wgpu::Sampler
         entry.sampler = resource.Get(); // Replace Get() with actual method to retrieve the sampler handle
      } else if constexpr (Binding::bindingType == BindingType::Texture) {
         // Assuming ToBind<T> is wgpu::TextureView
         entry.textureView = resource.Get(); // Replace Get() with actual method to retrieve the texture view handle
      }
      return entry;
   }
   // ========================

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
      }


      return entry;
   }
};

namespace wgpu {
constexpr wgpu::ShaderStage both(wgpu::ShaderStage lhs, wgpu::ShaderStage rhs) {
   return wgpu::ShaderStage(lhs.m_raw | rhs.m_raw);
}
} // namespace wgpu
