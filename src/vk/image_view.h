
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;

class ImageView {
 private:
  std::shared_ptr<vk::Device> device_;
  VkImage image_;
  VkFormat format_;
  VkImageView image_view_;
  VkImageAspectFlags aspect_mask_;

  void Initialize();
 public:
  static std::shared_ptr<ImageView> Create(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect_mask);

  explicit ImageView(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect_mask);

  ImageView(const ImageView&) = delete;
  ~ImageView();

  VkImageView Handle() const { return image_view_; }
};

}  // namespace vk
