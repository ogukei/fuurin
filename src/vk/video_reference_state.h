
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <vector>

namespace vk {

class Device;
class Queue;
class VideoProfile;
class VideoReferenceSlot;
class H264PictureParameters;

class VideoReferenceState {
 private:
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Queue> queue_;
  std::shared_ptr<vk::VideoProfile> video_profile_;
  std::shared_ptr<vk::H264PictureParameters> picture_parameters_;
  VkExtent2D extent_;
  VkFormat format_;

  std::vector<std::shared_ptr<vk::VideoReferenceSlot>> reference_slots_;

  void Initialize();

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

  const std::vector<std::shared_ptr<vk::VideoReferenceSlot>>& ReferenceSlots() const { return reference_slots_; }
};

}  // namespace vk
