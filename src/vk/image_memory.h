
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
  std::shared_ptr<vk::Device> device_;
 public:
  static std::shared_ptr<ImageMemory> Create(
    VkImage image,
    const std::shared_ptr<vk::Device>& device);

  explicit ImageMemory(VkImage image, const std::shared_ptr<vk::Device>& device);
  ImageMemory(const ImageMemory&) = delete;
  ~ImageMemory();
};

}  // namespace vk
