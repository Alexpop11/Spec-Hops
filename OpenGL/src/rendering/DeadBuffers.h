#include <vector>
#include <webgpu/webgpu.hpp>

class DeadBuffers {
   public:
      static std::vector<wgpu::Buffer> buffers;
};
