
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class VideoImage;

class VideoImageMemory {
 private:
  std::shared_ptr<vk::VideoImage> video_image_;
  VkDeviceMemory memory_;

  void Initialize();
 public:
  static std::shared_ptr<VideoImageMemory> Create(
    const std::shared_ptr<vk::VideoImage>& video_image);

  explicit VideoImageMemory(
    const std::shared_ptr<vk::VideoImage>& video_image);

  VideoImageMemory(const VideoImageMemory&) = delete;
  ~VideoImageMemory();
};

}  // namespace vk
