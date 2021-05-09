
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class DeviceQueue;

class Device {
 private:
  std::shared_ptr<vk::DeviceQueue> device_queue_;
  VkDevice device_;
 public:
  explicit Device(const std::shared_ptr<vk::DeviceQueue>&);
  ~Device();

  void Initialize(VkDevice device);
  VkDevice Handle() const { return device_; }
};

}  // namespace vk
