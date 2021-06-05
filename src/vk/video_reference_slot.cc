
#include "vk/video_reference_slot.h"

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/video_image.h"
#include "vk/video_image_memory.h"
#include "vk/video_frame.h"
#include "vk/image_view.h"

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
      index_(index) {
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
  // reference slot
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

VkImageMemoryBarrier2KHR VideoReferenceSlot::ImageMemoryBarrier() const {
  VkImageMemoryBarrier2KHR image_memory_barrier = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
    .pNext = nullptr,
    .srcStageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
    .srcAccessMask = 0,
    .dstStageMask = VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
    .dstAccessMask = VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = frame_->Queue()->FamilyIndex(),
    .image = frame_->VideoImage()->Handle(),
    .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
  };
  return image_memory_barrier;
}

VideoReferenceSlot::~VideoReferenceSlot() {
}

}  // namespace vk
