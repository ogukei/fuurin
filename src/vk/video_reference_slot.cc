
#include "vk/video_reference_slot.h"

#include "vk/device.h"
#include "vk/video_image.h"
#include "vk/video_image_memory.h"

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
}

VideoReferenceSlot::~VideoReferenceSlot() {
}

}  // namespace vk
