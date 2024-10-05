#pragma once

#include <webgpu/webgpu.h>

WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options);
WGPUDevice  requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);
