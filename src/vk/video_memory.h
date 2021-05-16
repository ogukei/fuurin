
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;

class VideoSessionMemory {
 private:
  std::shared_ptr<vk::Device> device_;
  VkDeviceSize allocation_size_;
  uint32_t memory_type_bits_;
  VkDeviceMemory memory_;

  void Initialize();
 public:
  static std::shared_ptr<VideoSessionMemory> Create(
    const std::shared_ptr<vk::Device>& device,
    VkDeviceSize allocation_size,
    uint32_t memory_type_bits);

  explicit VideoSessionMemory(
    const std::shared_ptr<vk::Device>& device,
    VkDeviceSize allocation_size,
    uint32_t memory_type_bits);

  VideoSessionMemory(const VideoSessionMemory&) = delete;
  ~VideoSessionMemory();

  VkDeviceMemory Handle() const { return memory_; }
};

}  // namespace vk
