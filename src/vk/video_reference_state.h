
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <vector>
#include <optional>

namespace vk {

class Device;
class Queue;
class VideoProfile;
class VideoReferenceSlot;
class H264PictureParameters;
class H264PictureInfo;
class VideoSessionFrame;

class VideoReferenceState {
 private:
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Queue> queue_;
  std::shared_ptr<vk::VideoProfile> video_profile_;
  std::shared_ptr<vk::H264PictureParameters> picture_parameters_;
  VkExtent2D extent_;
  VkFormat format_;

  std::vector<std::shared_ptr<vk::VideoReferenceSlot>> slots_;

  std::optional<std::shared_ptr<vk::VideoReferenceSlot>> setup_reference_slot_;
  std::optional<VkVideoReferenceSlotKHR> setup_reference_slot_info_;

  std::vector<std::shared_ptr<vk::VideoReferenceSlot>> slots_in_use_;
  std::vector<VkVideoReferenceSlotKHR> reference_slots_info_vec_;

  std::vector<VkImageMemoryBarrier2KHR> image_memory_barrier_vec_;

  void Initialize();
  std::optional<std::shared_ptr<vk::VideoReferenceSlot>> FindAvailableSlot() const;

 public:
  static std::shared_ptr<VideoReferenceState> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkExtent2D extent,
    VkFormat format);

  explicit VideoReferenceState(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkExtent2D extent,
    VkFormat format);

  VideoReferenceState(const VideoReferenceState&) = delete;
  ~VideoReferenceState();

  // decode staging
  void BeginDecode(const std::shared_ptr<vk::H264PictureInfo>& picture_info);
  void EndDecode(const std::shared_ptr<vk::H264PictureInfo>& picture_info);

  // stage information
  const VkVideoReferenceSlotKHR& SetupReferenceSlotInfo() const { return setup_reference_slot_info_.value(); }
  VkVideoPictureResourceKHR PictureResourceDestination() const;
  const std::vector<VkVideoReferenceSlotKHR>& ReferenceSlotsVec() const { return reference_slots_info_vec_; }

  const std::vector<VkImageMemoryBarrier2KHR>& ImageMemoryBarriers() const { return image_memory_barrier_vec_; }

  // for debug
  const std::shared_ptr<vk::VideoReferenceSlot>& SetupReferenceSlot() const { return setup_reference_slot_.value(); }
};

}  // namespace vk
