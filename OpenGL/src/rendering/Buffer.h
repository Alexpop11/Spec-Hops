#pragma once

#include <webgpu/webgpu.hpp>
#include <vector>
#include <cassert>
#include <iostream>

// Template class for Buffer abstraction
template <typename T>
class Buffer {
public:
   // Constructor: Creates a buffer with specified usage and data
   Buffer(wgpu::Device& device, wgpu::Queue& queue, const std::vector<T>& data, wgpu::BufferUsage usage)
      : device_(device)
      , queue_(queue)
      , size_(data.size() * sizeof(T))
      , count_(data.size())
      , usage_(usage) {
      assert(!data.empty() && "Buffer data cannot be empty.");

      // Create buffer descriptor
      wgpu::BufferDescriptor bufferDesc = {};
      bufferDesc.size                   = size_;
      bufferDesc.usage                  = usage_;
      bufferDesc.mappedAtCreation       = false;

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
         buffer_.destroy();
      }
   }

   // Deleted copy constructor and assignment operator to prevent copying
   Buffer(const Buffer&)            = delete;
   Buffer& operator=(const Buffer&) = delete;

   // Move constructor and assignment operator
   Buffer(Buffer&& other) noexcept
      : device_(other.device_)
      , queue_(other.queue_)
      , buffer_(other.buffer_)
      , size_(other.size_)
      , count_(other.count_)
      , usage_(other.usage_) {
      other.buffer_ = nullptr;
      other.size_   = 0;
      other.count_  = 0;
   }

   Buffer& operator=(Buffer&& other) noexcept {
      if (this != &other) {
         if (buffer_) {
            buffer_.destroy();
         }
         device_ = other.device_;
         queue_  = other.queue_;
         buffer_ = other.buffer_;
         size_   = other.size_;
         count_  = other.count_;
         usage_  = other.usage_;

         other.buffer_ = nullptr;
         other.size_   = 0;
         other.count_  = 0;
      }
      return *this;
   }

   // Method to upload data to the buffer
   void upload(const std::vector<T>& data) {
      assert(data.size() * sizeof(T) <= size_ && "Data size exceeds buffer size.");

      queue_.writeBuffer(buffer_, 0, data.data(), data.size() * sizeof(T));
   }

   // Getter for the underlying wgpu::Buffer
   wgpu::Buffer&       get() { return buffer_; }
   const wgpu::Buffer& get() const { return buffer_; }

   // Getter for buffer size
   size_t size() const { return size_; }
   size_t count() const { return count_; }

private:
   wgpu::Device&     device_;
   wgpu::Queue&      queue_;
   wgpu::Buffer      buffer_;
   size_t            size_;
   size_t            count_;
   wgpu::BufferUsage usage_;
};
