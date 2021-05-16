
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class DeviceQueue;
class CommandBuffer;
class PhysicalDevice;

class Queue {
 private:
  std::weak_ptr<vk::DeviceQueue> device_queue_;
  VkQueue queue_;
  uint32_t family_index_;
 public:
  explicit Queue(const std::shared_ptr<DeviceQueue>& device_queue, uint32_t family_index);

  void Initialize(VkQueue queue);
  uint32_t FamilyIndex() const { return family_index_; }

  void SubmitThenWait(const std::shared_ptr<CommandBuffer>&);
};

}  // namespace vk
