
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class CommandPool;

class CommandBuffer {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  VkCommandBuffer command_buffer_;

  void Initialize();
 public:
  static std::optional<std::shared_ptr<CommandBuffer>> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  explicit CommandBuffer(const std::shared_ptr<vk::CommandPool>& command_pool);
  CommandBuffer(const CommandBuffer&) = delete;
  ~CommandBuffer();

  VkCommandBuffer Handle() const { return command_buffer_; }
};

}  // namespace vk
