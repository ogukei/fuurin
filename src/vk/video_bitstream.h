
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class CommandPool;

class VideoBitstreamBuffer {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;

  void Initialize();
 public:
  static std::shared_ptr<VideoBitstreamBuffer> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  explicit VideoBitstreamBuffer(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  VideoBitstreamBuffer(const VideoBitstreamBuffer&) = delete;
  ~VideoBitstreamBuffer();
};

}  // namespace vk
