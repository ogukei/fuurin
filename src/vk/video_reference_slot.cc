
#include "vk/video_reference_slot.h"

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/video_image.h"
#include "vk/video_image_memory.h"
#include "vk/video_frame.h"
#include "vk/image_view.h"

#include "vk/video_h264_dpb_entry.h"

namespace vk {

std::shared_ptr<VideoReferenceSlot> VideoReferenceSlot::Create(
    const std::shared_ptr<vk::VideoSessionFrame>& frame,
    uint32_t index) {
  auto reference_slot = std::make_shared<VideoReferenceSlot>(frame, index);
  reference_slot->Initialize();
  return reference_slot;
}

VideoReferenceSlot::VideoReferenceSlot(
    const std::shared_ptr<vk::VideoSessionFrame>& frame,
    uint32_t index)
    : frame_(frame),
      index_(index),
      is_used_(false),
      image_layout_(VK_IMAGE_LAYOUT_UNDEFINED) {
}

void VideoReferenceSlot::Initialize() {
  extent_ = VkExtent2D {
    .width = frame_->Width(),
    .height = frame_->Height(),
  };
  // picture resource
  {
    VkVideoPictureResourceKHR picture_resource = {
      .sType = VK_STRUCTURE_TYPE_VIDEO_PICTURE_RESOURCE_KHR,
      .pNext = nullptr,
      .codedOffset = {},
      .codedExtent = extent_,
      .baseArrayLayer = 0,
      .imageViewBinding = frame_->ImageView()->Handle(),
    };
    picture_resource_ = picture_resource;
  }
  {
    VkVideoReferenceSlotKHR reference_slot = {
      .sType = VK_STRUCTURE_TYPE_VIDEO_REFERENCE_SLOT_KHR,
      .pNext = nullptr,
      .slotIndex = static_cast<int8_t>(index_),
      .pPictureResource = &picture_resource_,
    };
    reference_slot_ = reference_slot;
  }
}

VkImageMemoryBarrier2KHR VideoReferenceSlot::ImageMemoryBarrierAsReference() const {
  VkImageMemoryBarrier2KHR image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
    .pNext = nullptr,
    .srcStageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
    .srcAccessMask = 0,
    .dstStageMask = VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
    .dstAccessMask = VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR,
    .oldLayout = image_layout_,
    .newLayout = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = frame_->Queue()->FamilyIndex(),
    .image = frame_->VideoImage()->Handle(),
    .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
  };
  return image_memory_barrier;
}

VkImageMemoryBarrier2KHR VideoReferenceSlot::ImageMemoryBarrierAsSetupReference() const {
  VkImageMemoryBarrier2KHR image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
    .pNext = nullptr,
    .srcStageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
    .srcAccessMask = 0,
    .dstStageMask = VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
    .dstAccessMask = VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR,
    .oldLayout = image_layout_,
    .newLayout = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = frame_->Queue()->FamilyIndex(),
    .image = frame_->VideoImage()->Handle(),
    .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
  };
  return image_memory_barrier;
}

void VideoReferenceSlot::ConfigureAsSetupReference() {
  VkVideoReferenceSlotKHR reference_slot = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_REFERENCE_SLOT_KHR,
    .pNext = nullptr,
    .slotIndex = static_cast<int8_t>(index_),
    .pPictureResource = &picture_resource_,
  };
  reference_slot_ = reference_slot;
  reference_frame_index_ = std::nullopt;
}

void VideoReferenceSlot::ConfigureAsReference(const vk::H264DecodedPictureBufferEntry& entry) {
  // @see https://vulkan.lunarg.com/doc/view/1.2.176.1/linux/chunked_spec/chap39.html
  StdVideoDecodeH264ReferenceInfo std_reference_info = {
    .FrameNum = static_cast<uint16_t>(entry.frame_index),
    .reserved = 0,
    .PicOrderCnt = { entry.top_field_order_count, entry.bottom_field_order_count },
    .flags = {
      .top_field_flag = entry.is_top_field_flag ? (uint32_t)1 :(uint32_t)0,
      .bottom_field_flag = entry.is_bottom_field_flag ? (uint32_t)1 : (uint32_t)0,
      .is_long_term = entry.is_long_term ? (uint32_t)1 : (uint32_t)0,
      .is_non_existing = entry.is_non_existing ? (uint32_t)1 : (uint32_t)0
    }
  };
  std_reference_info_ = std_reference_info;
  VkVideoDecodeH264DpbSlotInfoEXT h264_dpb_slot_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_DPB_SLOT_INFO_EXT,
    .pNext = nullptr,
    .pStdReferenceInfo = &std_reference_info_.value(),
  };
  h264_dpb_slot_info_ = h264_dpb_slot_info;
  //
  VkVideoReferenceSlotKHR reference_slot = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_REFERENCE_SLOT_KHR,
    .pNext = &h264_dpb_slot_info_,
    .slotIndex = static_cast<int8_t>(index_),
    .pPictureResource = &picture_resource_,
  };
  reference_slot_ = reference_slot;
}

VideoReferenceSlot::~VideoReferenceSlot() {
}

}  // namespace vk
