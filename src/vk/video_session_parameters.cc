
#include "vk/video_session_parameters.h"

#include <iostream>

#include "vk/device.h"
#include "vk/video_h264_picture_parameters.h"

namespace vk {

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateVideoSessionParametersKHR.html
static PFN_vkCreateVideoSessionParametersKHR vk_vkCreateVideoSessionParametersKHR(VkDevice device) {
    return (PFN_vkCreateVideoSessionParametersKHR)vkGetDeviceProcAddr(device, "vkCreateVideoSessionParametersKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkDestroyVideoSessionParametersKHR.html
static PFN_vkDestroyVideoSessionParametersKHR vk_vkDestroyVideoSessionParametersKHR(VkDevice device) {
    return (PFN_vkDestroyVideoSessionParametersKHR)vkGetDeviceProcAddr(device, "vkDestroyVideoSessionParametersKHR");
}

std::shared_ptr<VideoSessionParameters> VideoSessionParameters::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkVideoSessionKHR video_session) {
  auto session_parameters = std::make_shared<VideoSessionParameters>(device, picture_parameters, video_session);
  session_parameters->Initialize();
  return session_parameters;
}

VideoSessionParameters::VideoSessionParameters(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters,
    VkVideoSessionKHR video_session)
    : device_(device),
      video_session_(video_session),
      video_session_parameters_(nullptr),
      picture_parameters_(picture_parameters) {
}

void VideoSessionParameters::Initialize() {
  // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoDecodeH264SessionParametersCreateInfoEXT.html

  // @see https://ffmpeg.org/doxygen/0.7/h264_8h-source.html
  VkVideoDecodeH264SessionParametersAddInfoEXT h264_add = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_SESSION_PARAMETERS_ADD_INFO_EXT,
    .pNext = nullptr,
    .spsStdCount = 1,
    .pSpsStd = &picture_parameters_->SequenceParameterSet().value(),
    .ppsStdCount = 1,
    .pPpsStd = &picture_parameters_->PictureParameterSet().value(),
  };
  VkVideoDecodeH264SessionParametersCreateInfoEXT h264 = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_SESSION_PARAMETERS_CREATE_INFO_EXT,
    .pNext = nullptr,
    .maxSpsStdCount = 32,
    .maxPpsStdCount = 256,
    .pParametersAddInfo = &h264_add,
  };
  VkVideoSessionParametersCreateInfoKHR create_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_SESSION_PARAMETERS_CREATE_INFO_KHR,
    .pNext = &h264,
    .videoSessionParametersTemplate = nullptr,
    .videoSession = video_session_,
  };
  VkVideoSessionParametersKHR video_session_parameters = nullptr;
  vk_vkCreateVideoSessionParametersKHR(device_->Handle())(
    device_->Handle(),
    &create_info,
    nullptr,
    &video_session_parameters);
  video_session_parameters_ = video_session_parameters;
}

VideoSessionParameters::~VideoSessionParameters() {
  vk_vkDestroyVideoSessionParametersKHR(device_->Handle())(
    device_->Handle(), video_session_parameters_, nullptr);
  video_session_parameters_ = nullptr;
}

}  // namespace vk
