
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

class VideoImage {
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
  static std::shared_ptr<VideoImage> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format);

  explicit VideoImage(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format);
  VideoImage(const VideoImage&) = delete;
  ~VideoImage();

  VkImage Handle() const { return image_; }

  const std::shared_ptr<vk::Device>& Device() const { return device_; }
};

}  // namespace vk
