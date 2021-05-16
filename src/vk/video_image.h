
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class Queue;
class ImageMemory;
class VideoProfile;

class VideoSessionImage {
 private:
  uint32_t width_;
  uint32_t height_;
  VkFormat format_;
  VkImage image_;
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Queue> queue_;
  std::shared_ptr<vk::VideoProfile> video_profile_;

  void Initialize();

 public:
  static std::shared_ptr<VideoSessionImage> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format);

  explicit VideoSessionImage(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format);
  VideoSessionImage(const VideoSessionImage&) = delete;
  ~VideoSessionImage();

  VkImage Handle() const { return image_; }
};

}  // namespace vk
