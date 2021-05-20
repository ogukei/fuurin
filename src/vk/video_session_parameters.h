
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;

class VideoSessionParameters {
 private:
  std::shared_ptr<vk::Device> device_;
  VkVideoSessionKHR video_session_;
  VkVideoSessionParametersKHR video_session_parameters_;

  StdVideoH264SequenceParameterSet h264_sequence_parameter_set_;
  StdVideoH264PictureParameterSet h264_picture_parameter_set_;

  void Initialize();
 public:
  static std::shared_ptr<VideoSessionParameters> Create(
    const std::shared_ptr<vk::Device>& device,
    VkVideoSessionKHR video_session);

  explicit VideoSessionParameters(
    const std::shared_ptr<vk::Device>& device,
    VkVideoSessionKHR video_session);

  VideoSessionParameters(const VideoSessionParameters&) = delete;
  ~VideoSessionParameters();

  VkVideoSessionParametersKHR Handle() const { return video_session_parameters_; }
};

}  // namespace vk
