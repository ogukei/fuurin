
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class DeviceQueue;

class Queue {
 private:
  std::shared_ptr<DeviceQueue> device_queue_;
  VkQueue queue_;
 public:
  explicit Queue(const std::shared_ptr<DeviceQueue>&);

  void Initialize(VkQueue queue);
  uint32_t FamilyIndex() const;
};

}  // namespace vk
