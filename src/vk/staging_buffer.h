
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class StagingBuffer {
 private:

  void Initialize();
 public:

  StagingBuffer();
  StagingBuffer(const StagingBuffer&) = delete;
  ~StagingBuffer();
};

}  // namespace vk
