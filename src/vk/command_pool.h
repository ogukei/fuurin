
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class Device;
class Queue;

class CommandPool {
 private:
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Queue> queue_;
  VkCommandPool command_pool_;

  bool Initialize();
 public:
  static std::optional<std::shared_ptr<CommandPool>> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue);

  explicit CommandPool(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue);

  CommandPool(const CommandPool &) = delete;
  ~CommandPool();

  VkCommandPool Handle() const { return command_pool_; }

  const std::shared_ptr<vk::Device>& Device() const { return device_; }
  const std::shared_ptr<vk::Queue>& Queue() const { return queue_; }
};

}  // namespace vk
