
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
  std::shared_ptr<vk::Queue> graphics_queue_;
  std::shared_ptr<vk::Queue> video_queue_;
  std::shared_ptr<vk::Device> device_;

  bool Initialize(const std::shared_ptr<DeviceQueue>&);
 public:
  static std::optional<std::shared_ptr<DeviceQueue>> Create(const std::shared_ptr<vk::PhysicalDevice>&);

  explicit DeviceQueue(const std::shared_ptr<vk::PhysicalDevice>&);
  DeviceQueue(const DeviceQueue&) = delete;
  ~DeviceQueue();

  const std::shared_ptr<vk::PhysicalDevice>& PhysicalDevice() const { return physical_device_; }
  const std::shared_ptr<vk::Device>& Device() const { return device_; }

  const std::shared_ptr<vk::Queue>& GraphicsQueue() const { return graphics_queue_; }
  const std::shared_ptr<vk::Queue>& VideoQueue() const { return video_queue_; }
};

}  // namespace vk
