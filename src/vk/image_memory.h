
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;

class ImageMemory {
 private:
  void Initialize();
  VkImage image_;
  VkDeviceMemory memory_;
  VkMemoryPropertyFlags memory_property_flags_;

  std::shared_ptr<vk::Device> device_;
 public:
  static std::shared_ptr<ImageMemory> Create(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkMemoryPropertyFlags memory_property_flags);

  explicit ImageMemory(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkMemoryPropertyFlags memory_property_flags);

  ImageMemory(const ImageMemory&) = delete;
  ~ImageMemory();

  VkDeviceMemory Handle() const { return memory_; }
};

}  // namespace vk
