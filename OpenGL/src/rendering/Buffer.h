#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <cassert>
#include <iostream>
#include <cstring> // For std::memcpy
#include <memory>  // For std::shared_ptr and std::weak_ptr

#include "../Application.h"

static std::vector<wgpu::Buffer> dead_buffers = {};

template <typename T, bool Uniform>
class BufferView;


// Memoization stuff
// -----------------------------------------
// Key structure to hold the data and usage
template <typename T>
struct Key {
   std::vector<T>  data;
   WGPUBufferUsage usage;

   bool operator==(const Key& other) const { return data == other.data && usage == other.usage; }
};

// Custom hash function for Key
template <typename T>
struct KeyHash {
   std::size_t operator()(const Key<T>& k) const {
      std::size_t h1 = std::hash<WGPUBufferUsage>{}(k.usage);
      std::size_t h2 = 0;
      for (const auto& elem : k.data) {
         h2 ^= std::hash<T>{}(elem) + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
      }
      return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
   }
};
// -----------------------------------------

template <typename T, bool Uniform = false>
class Buffer : public std::enable_shared_from_this<Buffer<T, Uniform>> {
public:
   // Friend declaration to allow BufferView access to private members
   friend class BufferView<T, Uniform>;

   // Constructor: Creates a buffer with specified usage and data
   Buffer(const std::vector<T>& data, wgpu::BufferUsage usage)
      : device_(Application::get().getDevice())
      , queue_(Application::get().getQueue())
      , usage_(usage) {
      std::cout << "Creating buffer..." << std::endl;

      count_    = data.size();
      capacity_ = count_;

      // Create buffer descriptor
      wgpu::BufferDescriptor bufferDesc = {};
      bufferDesc.usage                  = usage_;
      bufferDesc.mappedAtCreation       = false;
      bufferDesc.size                   = capacityBytes();

      // Create the buffer
      buffer_ = device_.createBuffer(bufferDesc);
      if (!buffer_) {
         std::cerr << "Failed to create buffer." << std::endl;
         return;
      }

      // Upload data to the buffer
      upload(data);
   }

   // Destructor: Releases the buffer resource
   ~Buffer() {
      if (buffer_) {
         dead_buffers.push_back(buffer_);
      }
   }

   // Deleted copy constructor and assignment operator to prevent copying
   Buffer(const Buffer&)            = delete;
   Buffer& operator=(const Buffer&) = delete;
   // Move constructor and assignment operator
   Buffer(Buffer&& other)            = delete;
   Buffer& operator=(Buffer&& other) = delete;

   // Method to upload data to the buffer
   void upload(const std::vector<T>& data) {
      assert(data.size() <= count_ && "Data size exceeds buffer capacity.");

      if constexpr (Uniform) {
         // Create a temporary buffer with padding
         std::vector<uint8_t> paddedData(capacityBytes(), 0); // Initialize with zeros
         for (size_t i = 0; i < data.size(); ++i) {
            // Copy each element into the padded buffer at the correct offset
            std::memcpy(&paddedData[i * elementStride()], &data[i], sizeof(T));
         }
         // Upload the padded data to the GPU buffer
         queue_.writeBuffer(buffer_, 0, paddedData.data(), capacityBytes());
      } else {
         // No padding needed; upload data directly
         queue_.writeBuffer(buffer_, 0, data.data(), data.size() * sizeof(T));
      }
   }

   size_t index(size_t index) const { return index * elementStride(); }

   // Getter for the underlying wgpu::Buffer
   wgpu::Buffer&       get() { return buffer_; }
   const wgpu::Buffer& get() const { return buffer_; }

   // Getter for buffer size
   size_t count() const { return count_; }

   // Add method: Allocates a new index and returns a BufferView
   BufferView<T, Uniform> Add(const T& data) {
      size_t allocatedIndex;

      // Reuse a deleted index if available
      if (!freeIndices_.empty()) {
         allocatedIndex = freeIndices_.back();
         freeIndices_.pop_back();
      } else {
         // Check if there's space in the current buffer
         if (count_ < capacity_) {
            allocatedIndex = count_;
            ++count_;
         } else {
            // Need to resize the buffer
            expandBuffer();
            allocatedIndex = count_;
            ++count_;
         }
      }

      // Update the buffer with the new data
      updateBuffer(data, allocatedIndex);

      // Return a BufferView managing this index
      return BufferView<T, Uniform>(this->shared_from_this(), allocatedIndex);
   }

   size_t sizeBytes() const { return count_ * elementStride(); }
   size_t capacityBytes() const { return capacity_ * elementStride(); }

   static std::shared_ptr<Buffer<T, Uniform>> create(const std::vector<T>& data, wgpu::BufferUsage usage) {
      // Static unordered_map with custom key and hash function
      static std::unordered_map<Key<T>, std::shared_ptr<Buffer<T, Uniform>>, KeyHash<T>> bufferMap;

      Key<T> key{data, usage};
      auto   it = bufferMap.find(key);
      if (it != bufferMap.end()) {
         // Return the existing shared_ptr if found
         return it->second;
      } else {
         // Create a new buffer and insert it into the map
         auto buffer    = std::make_shared<Buffer<T, Uniform>>(data, usage);
         bufferMap[key] = buffer;
         return buffer;
      }
   }

private:
   // Method to update data at a specific index
   void updateBuffer(const T& data, size_t index) {
      queue_.writeBuffer(buffer_, index * elementStride(), &data, sizeof(T));
   }

   // Method to resize the buffer
   void expandBuffer() {
      size_t newSize = capacityBytes() * 2;
      newSize        = std::max(newSize, elementStride()); // Ensure the buffer is at least 256 bytes

      // Create a new buffer with the new size
      wgpu::BufferDescriptor newBufferDesc = {};
      newBufferDesc.size                   = newSize;
      newBufferDesc.usage                  = usage_ | wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc;
      newBufferDesc.mappedAtCreation       = false;

      wgpu::Buffer newBuffer = device_.createBuffer(newBufferDesc);
      if (!newBuffer) {
         std::cerr << "Failed to create resized buffer." << std::endl;
         return;
      }

      // Create a command encoder
      // TODO: Should probably not be recreating the encoder every time we want to resize the buffer
      // Instead, maybe we should store the encoder in Application and reuse it within a frame, or accumulate a queue
      // of things to add and add them all once we call `flush`
      // (which would also minimize wasted copies when resizing multiple times within a frame)
      wgpu::CommandEncoder encoder = device_.createCommandEncoder();

      // Copy existing data from old buffer to new buffer
      encoder.copyBufferToBuffer(buffer_, 0, newBuffer, 0, capacityBytes());

      // Finish encoding and submit the commands
      wgpu::CommandBuffer commands = encoder.finish();
      queue_.submit(1, &commands);

      // Destroy the old buffer and replace it with the new buffer
      dead_buffers.push_back(buffer_);
      buffer_ = newBuffer;

      // Update buffer capaticy now that it's been resized
      capacity_ = newSize / elementStride();
   }

   // Method to free an index (called by BufferView destructor)
   void freeIndex(size_t index) { freeIndices_.push_back(index); }

   wgpu::Device&     device_;
   wgpu::Queue&      queue_;
   wgpu::Buffer      buffer_;
   size_t            count_ = 0;
   wgpu::BufferUsage usage_;
   static size_t     elementStride() {
      if constexpr (Uniform) {
         // Each element must start at a 256-byte boundary
         // So we round up sizeOf(T) to the nearest multiple of 256
         return (sizeof(T) + 255) & ~255;
      } else {
         return sizeof(T);
      }
   }
   // Allocation management
   size_t              capacity_ = 0; // Tracks the number of allocated elements
   std::vector<size_t> freeIndices_;  // Tracks freed indices for reuse
};

using IndexBuffer = Buffer<uint16_t, false>;

template <typename T>
using UniformBuffer = Buffer<T, true>;


template <typename T, bool Uniform = true>
class BufferView {
public:
   // Constructor: Acquires an index from the Buffer
   BufferView(std::shared_ptr<Buffer<T, Uniform>> buffer, size_t index)
      : buffer_(buffer)
      , index_(index) {}

   // Destructor: Frees the index back to the Buffer
   ~BufferView() { buffer_->freeIndex(index_); }

   // Deleted copy constructor and assignment operator
   BufferView(const BufferView&)             = delete;
   BufferView& operator=(const BufferView&)  = delete;
   BufferView(BufferView&& other)            = delete;
   BufferView& operator=(BufferView&& other) = delete;

   // Update method to modify the data at this index
   void Update(const T& data) { buffer_->updateBuffer(data, index_); }

   // Getter for the index
   std::shared_ptr<Buffer<T, Uniform>> getBuffer() const { return buffer_; }
   size_t                              getIndex() const { return index_; }
   size_t                              getOffset() const { return index_ * buffer_->elementStride(); }

   static BufferView<T, Uniform> create(const T& data) {
      static std::shared_ptr<Buffer<T, Uniform>> buffer = std::make_shared<Buffer<T, Uniform>>(
         std::vector<T>{}, wgpu::BufferUsage::CopySrc | wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform);
      return buffer->Add(data);
   }

private:
   std::shared_ptr<Buffer<T, Uniform>> buffer_;
   size_t                              index_;
};

template <typename T>
using UniformBufferView = BufferView<T, true>;
