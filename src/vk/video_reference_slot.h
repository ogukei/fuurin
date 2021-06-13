
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class Device;
class VideoSessionFrame;
class H264PictureInfo;
struct H264DecodedPictureBufferEntry;

class VideoReferenceSlot {
 private:
  std::shared_ptr<vk::VideoSessionFrame> frame_;
  uint32_t index_;
  VkExtent2D extent_;
  VkVideoPictureResourceKHR picture_resource_;
  VkVideoReferenceSlotKHR reference_slot_;

  std::optional<VkVideoDecodeH264DpbSlotInfoEXT> h264_dpb_slot_info_;
  std::optional<StdVideoDecodeH264ReferenceInfo> std_reference_info_;

  bool is_used_;
  std::optional<uint32_t> reference_frame_index_;
  VkImageLayout image_layout_;

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
  VkImageMemoryBarrier2KHR ImageMemoryBarrierAsReference() const;
  VkImageMemoryBarrier2KHR ImageMemoryBarrierAsSetupReference() const;

  const VkVideoPictureResourceKHR& VideoPictureResource() const { return picture_resource_; }
  const VkVideoReferenceSlotKHR& Info() const { return reference_slot_; }

  void ConfigureAsSetupReference();
  void ConfigureAsReference(const vk::H264DecodedPictureBufferEntry& entry);

  void SetReferenceFrameIndex(uint32_t index) { reference_frame_index_ = index; }
  std::optional<uint32_t> ReferenceFrameIndex() const { return reference_frame_index_; }

  void SetImageLayout(VkImageLayout layout) { image_layout_ = layout; }
  VkImageLayout ImageLayout() const { return image_layout_; }

  void MarkInUse() { is_used_ = true; }
  void UnmarkInUse() { is_used_ = false; }
  bool IsAvailable() const { return !is_used_; }
};

}  // namespace vk
