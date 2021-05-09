
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class PhysicalDevice;
class Queue;
class Device;

class DeviceQueue {
 private:
  std::shared_ptr<vk::PhysicalDevice> physical_device_;
  std::shared_ptr<vk::Queue> queue_;
  std::optional<uint32_t> queue_family_index_;
  std::shared_ptr<vk::Device> device_;
 public:
  static std::optional<std::shared_ptr<DeviceQueue>> Create(const std::shared_ptr<vk::PhysicalDevice>&);

  explicit DeviceQueue(const std::shared_ptr<vk::PhysicalDevice>&);
  DeviceQueue(const DeviceQueue&) = delete;
  ~DeviceQueue();

  bool Initialize(const std::shared_ptr<DeviceQueue>&);

  uint32_t QueueFamilyIndex() const { return queue_family_index_.value(); }

  const std::shared_ptr<vk::PhysicalDevice>& PhysicalDevice() const { return physical_device_; }
  const std::shared_ptr<vk::Device>& Device() const { return device_; }
  const std::shared_ptr<vk::Queue>& Queue() const { return queue_; }
};

}  // namespace vk
