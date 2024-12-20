#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <type_traits>
#include "Buffer.h"
#include "Texture.h"
#include "TextureSampler.h"
#include "BindGroup.h"
#include <array>

enum class BindingType {
   Buffer,
   Sampler,
   Texture,
};

template <typename Binding>
struct GetToBind;

// Helper struct for buffer bindings (e.g., Uniform or Storage buffers)
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType, bool DynamicOffset>
struct BufferBinding {
   using Type                                             = T;
   static constexpr wgpu::ShaderStage       visibility    = Visibility;
   static constexpr wgpu::BufferBindingType bufferType    = BufferType;
   static constexpr BindingType             bindingType   = BindingType::Buffer;
   static constexpr bool                    dynamicOffset = DynamicOffset;
};
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType>
struct GetToBind<BufferBinding<T, Visibility, BufferType, false>> {
   using type = const std::tuple<const Buffer<T, BufferType == wgpu::BufferBindingType::Uniform>&, size_t>;
};
template <typename T, wgpu::ShaderStage Visibility, wgpu::BufferBindingType BufferType>
struct GetToBind<BufferBinding<T, Visibility, BufferType, true>> {
   using type = const BufferView<T, BufferType == wgpu::BufferBindingType::Uniform>&;
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
   using type = const TextureSampler&;
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
   using type = Texture* const;
};

template <typename Binding>
using ToBind = typename GetToBind<Binding>::type;
template <typename T>
concept BindingC = requires { typename ToBind<T>; };

// Helper for caching
// ------------------------------------------------------
template <std::size_t N>
using Ids = std::array<std::array<int32_t, 3>, N>;
template <std::size_t N>
struct IdsHash {
   std::size_t operator()(const Ids<N>& ids) const {
      std::size_t seed = 0;
      for (const auto& pair : ids) {
         seed ^= std::hash<int32_t>{}(pair[0]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
         seed ^= std::hash<int32_t>{}(pair[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      return seed;
   }
};


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
   static BindGroup ToBindGroup(wgpu::Device& device, ToBind<Bindings>&... resources) {
      // Create a tuple of references to the resources
      auto resourcesTuple = std::forward_as_tuple(resources...);

      // Create the bind group using the resources
      BindGroup bindGroup = createBindGroup(device, resourcesTuple, std::index_sequence_for<Bindings...>{});

      return bindGroup;
   }

   template <typename Tuple, size_t... I>
   static BindGroup createBindGroup(wgpu::Device& device, Tuple& resources, std::index_sequence<I...>) {
      // Get the ids of the binding resources
      auto ids = Ids<sizeof...(Bindings)>{getId<I, Bindings>(std::get<I>(resources))...};

      static std::unordered_map<Ids<sizeof...(Bindings)>, BindGroup, IdsHash<sizeof...(Bindings)>> bindGroupsCache;
      // check if ids is in the cache
      if (auto it = bindGroupsCache.find(ids); it != bindGroupsCache.end()) {
         return it->second;
      }

      // Create an array of BindGroupEntry
      std::array<wgpu::BindGroupEntry, sizeof...(Bindings)> entries = {
         createBindGroupEntry<I, Bindings>(device, std::get<I>(resources))...};

      // Create the BindGroupDescriptor
      wgpu::BindGroupDescriptor bindGroupDesc{};
      bindGroupDesc.layout     = CreateLayout(device);
      bindGroupDesc.entryCount = static_cast<uint32_t>(entries.size());
      bindGroupDesc.entries    = entries.data();

      BindGroup bindgroup = BindGroup(device.createBindGroup(bindGroupDesc));
      bindGroupsCache.emplace(ids, bindgroup);
      return bindgroup;
   }

   template <size_t I, typename Binding, typename Resource>
   static std::array<int32_t, 3> getId(Resource& resource) {
      if constexpr (Binding::bindingType == BindingType::Buffer) {
         if constexpr (!Binding::dynamicOffset) {
            // Assuming ToBind<T> is Buffer
            return std::array<int32_t, 3>{std::get<0>(resource).summed_id(), 
                                        static_cast<int32_t>(std::get<1>(resource)),
                                        static_cast<int32_t>(std::get<0>(resource).count())};
         } else if constexpr (Binding::dynamicOffset) {
            return std::array<int32_t, 3>{resource.getBuffer()->summed_id(), -1, -1};
         }
      } else if constexpr (Binding::bindingType == BindingType::Sampler) {
         return std::array<int32_t, 3>{resource.id, -1, -1};
      } else if constexpr (Binding::bindingType == BindingType::Texture) {
         return std::array<int32_t, 3>{resource->id, -1, -1};
      }
      return std::array<int32_t, 3>{-1, -1, -1}; // Default case
   }

   template <size_t I, typename Binding, typename Resource>
   static wgpu::BindGroupEntry createBindGroupEntry(wgpu::Device& device, Resource& resource) {
      wgpu::BindGroupEntry entry{};
      entry.binding = static_cast<uint32_t>(I);
      if constexpr (Binding::bindingType == BindingType::Buffer) {
         if constexpr (!Binding::dynamicOffset) {
            // Assuming ToBind<T> is Buffer
            entry.buffer = std::get<0>(resource).get();
            entry.offset = std::get<1>(resource);
            entry.size   = sizeof(typename Binding::Type) * std::get<0>(resource).count();
         } else if constexpr (Binding::dynamicOffset) {
            entry.buffer = resource.getBuffer()->get();
            entry.offset = 0;
            entry.size   = sizeof(typename Binding::Type);
         }
      } else if constexpr (Binding::bindingType == BindingType::Sampler) {
         // Assuming ToBind<T> is TextureSampler
         entry.sampler = resource.getSampler();
      } else if constexpr (Binding::bindingType == BindingType::Texture) {
         // Assuming ToBind<T> is Texture
         entry.textureView = resource->getTextureView();
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

template <typename... Layouts>
struct BindGroupLayouts {
   static std::vector<wgpu::BindGroupLayout> CreateLayouts(wgpu::Device& device) {
      return {Layouts::CreateLayout(device)...};
   }

   // Ensure that the number of tuples matches the number of layouts
   template <typename... Tuples>
   static std::vector<BindGroup> BindGroups(wgpu::Device& device, Tuples&&... tuples) {
      static_assert(sizeof...(Layouts) == sizeof...(Tuples), "Number of layouts and tuples must match");

      // Create a vector and initialize it with the results of each BindGroup call
      return {call_bind_group<Layouts>(device, std::forward<Tuples>(tuples))...};
   }

private:
   // Helper function to call BindGroup for a single layout and tuple
   template <typename Layout, typename Tuple>
   static BindGroup call_bind_group(wgpu::Device& device, Tuple&& tuple) {
      // Unpack the tuple and pass the resources to Layout::BindGroup
      return std::apply([&device](auto&&... args) -> BindGroup { return Layout::ToBindGroup(device, args...); },
                        std::forward<Tuple>(tuple));
   }
};
