
#include "vk/video_reference_state.h"

#include <iostream>
#include <utility>
#include <algorithm>
#include <cassert>

#include "vk/video_frame.h"
#include "vk/video_image.h"
#include "vk/video_reference_slot.h"
#include "vk/video_h264_picture_parameters.h"
#include "vk/video_h264_picture_info.h"
#include "vk/video_h264_dpb_entry.h"

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
  // always reserve one for the setup reference slot
  size_t num_references = 1 + sps.max_num_ref_frames;
  slots_.resize(num_references);
  for (uint32_t i = 0; i < num_references; i++) {
    auto frame = VideoSessionFrame::Create(device_, queue_, video_profile_, extent_.width, extent_.height, format_);
    auto reference_slot = VideoReferenceSlot::Create(frame, i);
    slots_[i] = std::move(reference_slot);
  }
}

void VideoReferenceState::BeginDecode(const std::shared_ptr<vk::H264PictureInfo>& picture_info) {
  auto& dpb_entries = picture_info->DecodedPictureBufferEntries();
  // remove unnecessary slots
  {
    std::vector<uint32_t> used_frame_indices;
    for (auto& dpb_entry : dpb_entries) {
      used_frame_indices.push_back(dpb_entry.frame_index);
    }
    std::vector<std::shared_ptr<vk::VideoReferenceSlot>> unused_slots;
    {
      auto where_unused = [&](const std::shared_ptr<vk::VideoReferenceSlot>& slot) {
        uint32_t frame_index = slot->ReferenceFrameIndex().value();
        auto it = std::find(used_frame_indices.begin(), used_frame_indices.end(), frame_index);
        return (it == used_frame_indices.end());
      };
      std::copy_if(
          slots_in_use_.begin(),
          slots_in_use_.end(),
          std::back_inserter(unused_slots),
          where_unused);
    }
    std::vector<std::shared_ptr<vk::VideoReferenceSlot>> used_slots;
    {
      auto where_used = [&](const std::shared_ptr<vk::VideoReferenceSlot>& slot) {
        uint32_t frame_index = slot->ReferenceFrameIndex().value();
        auto it = std::find(used_frame_indices.begin(), used_frame_indices.end(), frame_index);
        return (it != used_frame_indices.end());
      };
      std::copy_if(
          slots_in_use_.begin(),
          slots_in_use_.end(),
          std::back_inserter(used_slots),
          where_used);
    }
    for (auto& unused_slot : unused_slots) {
      unused_slot->UnmarkInUse();
    }
    assert(used_slots.size() == used_frame_indices.size());
    slots_in_use_ = std::move(used_slots);
  }
  // configure used slots
  for (auto& slot : slots_in_use_) {
    // find associated entry
    std::optional<vk::H264DecodedPictureBufferEntry> associated_entry;
    for (auto& dpb_entry : dpb_entries) {
      if (dpb_entry.frame_index == slot->ReferenceFrameIndex()) {
        associated_entry = dpb_entry;
        break;
      }
    }
    auto& entry = associated_entry.value();
    slot->ConfigureAsReference(entry);
  }
  // maps info
  reference_slots_info_vec_.clear();
  for (auto& slot : slots_in_use_) {
    reference_slots_info_vec_.push_back(slot->Info());
  }
  // setup reference
  auto slot = FindAvailableSlot().value();
  slot->ConfigureAsSetupReference();
  slot->MarkInUse();
  slot->SetReferenceFrameIndex(picture_info->FrameIndex());
  setup_reference_slot_ = slot;
  setup_reference_slot_info_ = slot->Info();
  // image memory barriers
  auto& image_memory_barriers = image_memory_barrier_vec_;
  image_memory_barriers.clear();
  if (slot->ImageLayout() != VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR) {
    image_memory_barriers.push_back(slot->ImageMemoryBarrierAsSetupReference());
  }
  for (auto& dpb_slot : slots_in_use_) {
    if (dpb_slot->ImageLayout() != VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR) {
      image_memory_barriers.push_back(dpb_slot->ImageMemoryBarrierAsReference());
    }
  }
  // dump
  std::cout << "  Selected setup reference slot #" << static_cast<int>(slot->Info().slotIndex) << std::endl;
  for (auto& slot : slots_in_use_) {
    std::cout << "  DPB slot #" << static_cast<int>(slot->Info().slotIndex) << std::endl;
  }
}

void VideoReferenceState::EndDecode(const std::shared_ptr<vk::H264PictureInfo>& picture_info) {
  // dpb
  for (auto& dpb_slot : slots_in_use_) {
    dpb_slot->SetImageLayout(VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR);
  }
  // add a setup reference slot to the queue for the later reference
  auto& setup_reference_slot = setup_reference_slot_.value();
  setup_reference_slot->SetImageLayout(VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR);
  slots_in_use_.push_back(setup_reference_slot);
  // clear
  setup_reference_slot_info_ = std::nullopt;
}

VkVideoPictureResourceKHR VideoReferenceState::PictureResourceDestination() const {
  return setup_reference_slot_.value()->VideoPictureResource();
}

std::optional<std::shared_ptr<vk::VideoReferenceSlot>> VideoReferenceState::FindAvailableSlot() const {
  for (auto& slot : slots_) {
    if (slot->IsAvailable()) return slot;
  }
  return std::nullopt;
}

VideoReferenceState::~VideoReferenceState() {
}

}  // namespace vk
