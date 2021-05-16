
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class Queue;
class VideoProfile;
class VideoSessionImage;

class VideoSessionFrame {
 private:
  uint32_t width_;
  uint32_t height_;
  VkFormat format_;
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Queue> queue_;
  std::shared_ptr<vk::VideoProfile> video_profile_;
  std::shared_ptr<vk::VideoSessionImage> image_;

  void Initialize();

 public:
  static std::shared_ptr<VideoSessionFrame> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format);

  explicit VideoSessionFrame(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format);

  VideoSessionFrame(const VideoSessionFrame&) = delete;
  ~VideoSessionFrame();
};

}  // namespace vk
