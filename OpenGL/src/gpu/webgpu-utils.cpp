#include "webgpu-utils.h"
#include <webgpu/webgpu.h>
#include <iostream>
#include <cassert>

/**
 * Utility function to get a WebGPU adapter, so that
 *     WGPUAdapter adapter = requestAdapterSync(options);
 * is roughly equivalent to
 *     const adapter = await navigator.gpu.requestAdapter(options);
 */
WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options) {
   // A simple structure holding the local information shared with the
   // onAdapterRequestEnded callback.
   struct UserData {
      WGPUAdapter adapter      = nullptr;
      bool        requestEnded = false;
   };
   UserData userData;

   // Callback called by wgpuInstanceRequestAdapter when the request returns
   // This is a C++ lambda function, but could be any function defined in the
   // global scope. It must be non-capturing (the brackets [] are empty) so
   // that it behaves like a regular C function pointer, which is what
   // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
   // is to convey what we want to capture through the pUserData pointer,
   // provided as the last argument of wgpuInstanceRequestAdapter and received
   // by the callback as its last argument.
   auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message,
                                   void* pUserData) {
      UserData& userData = *reinterpret_cast<UserData*>(pUserData);
      if (status == WGPURequestAdapterStatus_Success) {
         userData.adapter = adapter;
      } else {
         std::cout << "Could not get WebGPU adapter: " << message << std::endl;
      }
      userData.requestEnded = true;
   };

   // Call to the WebGPU request adapter procedure
   wgpuInstanceRequestAdapter(instance /* equivalent of navigator.gpu */, options, onAdapterRequestEnded,
                              (void*)&userData);

   // We wait until userData.requestEnded gets true
   // [...] Wait for request to end

   assert(userData.requestEnded);

   return userData.adapter;
}

/**
 * Utility function to get a WebGPU device, so that
 *     WGPUAdapter device = requestDeviceSync(adapter, options);
 * is roughly equivalent to
 *     const device = await adapter.requestDevice(descriptor);
 * It is very similar to requestAdapter
 */
WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        onDeviceRequestEnded,
        (void*)&userData
    );

#ifdef __EMSCRIPTEN__
    while (!userData.requestEnded) {
        emscripten_sleep(100);
    }
#endif // __EMSCRIPTEN__

    assert(userData.requestEnded);

    return userData.device;
}

// We also add an inspect device function:
void inspectDevice(WGPUDevice device) {
    std::vector<WGPUFeatureName> features;
    size_t featureCount = wgpuDeviceEnumerateFeatures(device, nullptr);
    features.resize(featureCount);
    wgpuDeviceEnumerateFeatures(device, features.data());

    std::cout << "Device features:" << std::endl;
    std::cout << std::hex;
    for (auto f : features) {
        std::cout << " - 0x" << f << std::endl;
    }
    std::cout << std::dec;

    WGPUSupportedLimits limits = {};
    limits.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
    bool success = wgpuDeviceGetLimits(device, &limits) == WGPUStatus_Success;
#else
    bool success = wgpuDeviceGetLimits(device, &limits);
#endif

    if (success) {
        std::cout << "Device limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << limits.limits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << limits.limits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << limits.limits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << limits.limits.maxTextureArrayLayers << std::endl;
        // [...] Extra device limits
    }
}
