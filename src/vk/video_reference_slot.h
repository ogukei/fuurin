
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
  VkExtent2D extent_;
  VkVideoPictureResourceKHR picture_resource_;
  VkVideoReferenceSlotKHR reference_slot_;

  VkVideoDecodeH264DpbSlotInfoEXT h264_dpb_slot_info_;
  StdVideoDecodeH264ReferenceInfo std_reference_info_;

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
  VkImageMemoryBarrier2KHR ImageMemoryBarrier() const;

  const VkVideoPictureResourceKHR& VideoPictureResource() const { return picture_resource_; }
  const VkVideoReferenceSlotKHR& Handle() const { return reference_slot_; }
};

}  // namespace vk
