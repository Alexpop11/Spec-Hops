#pragma once

#include <webgpu/webgpu.hpp>

void wgpuPollEvents(wgpu::Device device, bool yieldToWebBrowser);
