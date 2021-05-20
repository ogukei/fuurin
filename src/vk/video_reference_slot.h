
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class VideoSessionFrame;

class VideoReferenceSlot {
 private:
  std::shared_ptr<vk::VideoSessionFrame> frame_;
  uint32_t index_;

  void Initialize();

 public:
  static std::shared_ptr<VideoReferenceSlot> Create(
    const std::shared_ptr<vk::VideoSessionFrame>& frame,
    uint32_t index);

  explicit VideoReferenceSlot(
    const std::shared_ptr<vk::VideoSessionFrame>& frame,
    uint32_t index);

  VideoReferenceSlot(const VideoReferenceSlot&) = delete;
  ~VideoReferenceSlot();

  const std::shared_ptr<vk::VideoSessionFrame>& Frame() const { return frame_; }
};

}  // namespace vk
