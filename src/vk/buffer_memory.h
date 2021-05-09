

#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class DeviceQueue;
class PhysicalDevice;

std::optional<uint32_t> MemoryTypeIndex(
    const std::shared_ptr<vk::PhysicalDevice> physical_device,
    uint32_t type_bits,
    VkMemoryPropertyFlags property_flags);

class BufferMemory {
 private:
  std::shared_ptr<vk::DeviceQueue> device_queue_;
  VkDeviceSize size_;
  VkBufferUsageFlags buffer_usage_flags_;
  VkMemoryPropertyFlags memory_property_flags_;

  VkBuffer buffer_;
  VkDeviceMemory memory_;

  bool Initialize();

 public:
  static std::optional<std::shared_ptr<BufferMemory>> Create(
      const std::shared_ptr<vk::DeviceQueue>& device_queue,
      VkDeviceSize size,
      VkBufferUsageFlags buffer_usage_flags,
      VkMemoryPropertyFlags memory_property_flags);

  explicit BufferMemory(
      const std::shared_ptr<vk::DeviceQueue>& device_queue,
      VkDeviceSize size,
      VkBufferUsageFlags buffer_usage_flags,
      VkMemoryPropertyFlags memory_property_flags);

  BufferMemory(const BufferMemory&) = delete;
  ~BufferMemory();
};

}  // namespace vk
