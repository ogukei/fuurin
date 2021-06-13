
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class VideoSessionFrame;

class VideoDecodeSurface {
 private:
  std::shared_ptr<vk::VideoSessionFrame> frame_;
  uint32_t index_;
  VkExtent2D extent_;

  void Initialize();

 public:
  static std::shared_ptr<VideoDecodeSurface> Create(
    const std::shared_ptr<vk::VideoSessionFrame>& frame,
    uint32_t index);

  explicit VideoDecodeSurface(
    const std::shared_ptr<vk::VideoSessionFrame>& frame,
    uint32_t index);

  VideoDecodeSurface(const VideoDecodeSurface&) = delete;
  ~VideoDecodeSurface();

  const std::shared_ptr<vk::VideoSessionFrame>& Frame() const { return frame_; }

  VkImageMemoryBarrier2KHR ImageMemoryBarrier() const;
  VkVideoPictureResourceKHR VideoPictureResource() const;
};

}  // namespace vk
