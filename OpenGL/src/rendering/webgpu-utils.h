#pragma once

#include <webgpu/webgpu.hpp>

void wgpuPollEvents(wgpu::Device device, bool yieldToWebBrowser);

namespace wgpu {
constexpr wgpu::ShaderStage bothShaderStages(wgpu::ShaderStage lhs, wgpu::ShaderStage rhs) {
   return wgpu::ShaderStage(WGPUShaderStage(lhs.m_raw | rhs.m_raw));
}
constexpr wgpu::BufferUsage bothBufferUsages(wgpu::BufferUsage lhs, wgpu::BufferUsage rhs) {
   return wgpu::BufferUsage(WGPUBufferUsage(lhs.m_raw | rhs.m_raw));
}
constexpr wgpu::BufferUsage bothBufferUsages(wgpu::BufferUsage lhs, wgpu::BufferUsage rhs, wgpu::BufferUsage rhs2) {
   return wgpu::BufferUsage(WGPUBufferUsage(lhs.m_raw | rhs.m_raw | rhs2.m_raw));
}
constexpr wgpu::BufferUsage bothBufferUsages(wgpu::BufferUsage lhs, wgpu::BufferUsage rhs, wgpu::BufferUsage rhs2,
                                             wgpu::BufferUsage rhs3) {
   return wgpu::BufferUsage(WGPUBufferUsage(lhs.m_raw | rhs.m_raw | rhs2.m_raw | rhs3.m_raw));
}
} // namespace wgpu
