#include "webgpu-utils.h"
#include <iostream>
#include <cassert>

#include <webgpu/webgpu.hpp>

// We define a function that hides implementation-specific variants of device polling:
void wgpuPollEvents([[maybe_unused]] wgpu::Device device, [[maybe_unused]] bool yieldToWebBrowser) {
#if defined(WEBGPU_BACKEND_DAWN)
    device.tick();
#elif defined(WEBGPU_BACKEND_WGPU)
    device.poll(false);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
    if (yieldToWebBrowser) {
        emscripten_sleep(100);
    }
#endif
}
