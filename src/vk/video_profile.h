
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;

class VideoProfile {
 private:
  VkVideoProfileKHR profile_;
  VkVideoProfileKHR profile_with_ext_;
  VkVideoDecodeH264ProfileEXT decode_h264_profile_;
  std::shared_ptr<vk::Device> device_;

  void InitializeH264Decode();
 public:
  static std::shared_ptr<VideoProfile> CreateH264Decode(
    const std::shared_ptr<vk::Device>& device);

  explicit VideoProfile(
    const std::shared_ptr<vk::Device>& device);

  VideoProfile(const VideoProfile&) = delete;
  ~VideoProfile();

  const VkVideoProfileKHR& Profile() const { return profile_; }
  // FIXME: vkCreateImage does not need ext. session creation probably needs ext
  const VkVideoProfileKHR& ProfileWithExt() const { return profile_with_ext_; }
};

}  // namespace vk
