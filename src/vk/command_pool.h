
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class DeviceQueue;

class CommandPool {
 private:
  std::shared_ptr<vk::DeviceQueue> device_queue_;
  VkCommandPool command_pool_;

  bool Initialize();
 public:
  static std::optional<std::shared_ptr<CommandPool>> Create(
      const std::shared_ptr<vk::DeviceQueue>& device_queue);

  explicit CommandPool(const std::shared_ptr<vk::DeviceQueue>& device_queue);
  CommandPool(const CommandPool &) = delete;
  ~CommandPool();

  VkCommandPool Handle() const { return command_pool_; }
  const std::shared_ptr<vk::DeviceQueue>& DeviceQueue() const { return device_queue_; }
};

}  // namespace vk
