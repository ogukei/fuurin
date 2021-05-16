
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class DeviceQueue;
class PhysicalDevice;

class Device {
 private:
  std::weak_ptr<vk::DeviceQueue> device_queue_;
  std::shared_ptr<vk::PhysicalDevice> physical_device_;

  VkDevice device_;
 public:
  explicit Device(const std::shared_ptr<vk::DeviceQueue>&);
  ~Device();

  void Initialize(VkDevice device);
  VkDevice Handle() const { return device_; }

  const std::weak_ptr<vk::DeviceQueue>& DeviceQueue() const { return device_queue_; }
  const std::shared_ptr<vk::PhysicalDevice>& PhysicalDevice() const { return physical_device_; }
};

}  // namespace vk
