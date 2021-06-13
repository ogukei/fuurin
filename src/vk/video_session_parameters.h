
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class H264PictureParameters;

class VideoSessionParameters {
 private:
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::H264PictureParameters> picture_parameters_;

  VkVideoSessionKHR video_session_;
  VkVideoSessionParametersKHR video_session_parameters_;

  void Initialize();
 public:
  static std::shared_ptr<VideoSessionParameters> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkVideoSessionKHR video_session);

  explicit VideoSessionParameters(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkVideoSessionKHR video_session);

  VideoSessionParameters(const VideoSessionParameters&) = delete;
  ~VideoSessionParameters();

  VkVideoSessionParametersKHR Handle() const { return video_session_parameters_; }
};

}  // namespace vk
