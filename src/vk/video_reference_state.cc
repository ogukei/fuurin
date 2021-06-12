
#include "vk/video_reference_state.h"

#include <iostream>
#include <utility>

#include "vk/video_frame.h"
#include "vk/video_image.h"
#include "vk/video_reference_slot.h"
#include "vk/video_h264_picture_parameters.h"

namespace vk {

std::shared_ptr<VideoReferenceState> VideoReferenceState::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkExtent2D extent,
    VkFormat format) {
  auto reference_state = std::make_shared<VideoReferenceState>(
    device,
    queue,
    video_profile,
    picture_parameters,
    extent,
    format);
  reference_state->Initialize();
  return reference_state;
}

VideoReferenceState::VideoReferenceState(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkExtent2D extent,
    VkFormat format)
    : device_(device),
      queue_(queue),
      video_profile_(video_profile),
      picture_parameters_(picture_parameters),
      extent_(extent),
      format_(format) {
}

void VideoReferenceState::Initialize() {
  std::cout << "VideoReferenceState::Initialize()" << std::endl;
  auto& sps = picture_parameters_->SequenceParameterSet().value();
  //sps.max_num_ref_frames;

  uint32_t num_dpb_references = 4;
  reference_slots_.resize(num_dpb_references);
  for (uint32_t i = 0; i < num_dpb_references; i++) {
    auto frame = VideoSessionFrame::Create(device_, queue_, video_profile_, extent_.width, extent_.height, format_);
    auto reference_slot = VideoReferenceSlot::Create(frame, i);
    reference_slots_[i] = std::move(reference_slot);
  }
}

VideoReferenceState::~VideoReferenceState() {
}

}  // namespace vk
