
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class ImageMemory;

class Image {
 private:
  uint32_t width_;
  uint32_t height_;
  VkImageUsageFlags usage_;
  VkFormat format_;
  VkImageTiling tiling_;
  VkImage image_;
  std::shared_ptr<vk::Device> device_;

  void Initialize();

 public:
  static std::shared_ptr<Image> Create(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageTiling tiling);

  explicit Image(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageUsageFlags usage,
    VkImageTiling tiling);
  Image(const Image&) = delete;
  ~Image();

  VkImage Handle() const { return image_; }
};

}  // namespace vk
