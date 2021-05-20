
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
class VideoImage;
class VideoImageMemory;
class ImageView;

class VideoSessionFrame {
 private:
  uint32_t width_;
  uint32_t height_;
  VkFormat format_;
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Queue> queue_;
  std::shared_ptr<vk::VideoProfile> video_profile_;
  std::shared_ptr<vk::VideoImage> image_;
  std::shared_ptr<vk::VideoImageMemory> image_memory_;
  std::shared_ptr<vk::ImageView> image_view_;

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

  const std::shared_ptr<vk::ImageView>& ImageView() const { return image_view_; }
};

}  // namespace vk
