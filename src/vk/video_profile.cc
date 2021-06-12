
#include "vk/video_profile.h"

#include "vk/instance.h"
#include "vk/physical_device.h"
#include "vk/device_queue.h"
#include "vk/device.h"

namespace vk {

static PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(VkInstance instance) {
  return (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)vkGetInstanceProcAddr(
    instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
}

std::shared_ptr<VideoProfile> VideoProfile::CreateH264Decode(
    const std::shared_ptr<vk::Device>& device) {
  auto video_profile = std::make_shared<VideoProfile>(device);
  video_profile->InitializeH264Decode();
  return video_profile;
}

VideoProfile::VideoProfile(
    const std::shared_ptr<vk::Device>& device)
    : device_(device),
      profile_({}),
      profile_with_ext_({}),
      decode_h264_profile_({}) {
}

void VideoProfile::InitializeH264Decode() {
  auto& device = device_;
  auto& physical_device = device->PhysicalDevice();
  auto& instance = physical_device->Instance();
  // @see PopulateProfileExt
  // https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/nvVideoProfile.h#L55
  VkVideoProfileKHR video_profile = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_PROFILE_KHR,
    .pNext = nullptr,
    .videoCodecOperation = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT,
    // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoChromaSubsamplingFlagBitsKHR.html
    .chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
    // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoComponentBitDepthFlagBitsKHR.html
    .lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
    .chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
  };
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/nvVideoProfile.h#L55
  VkVideoDecodeH264ProfileEXT decode_h264_profile = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_EXT,
    .pNext = nullptr,
    // note that `std_video_h264_profile_idc_main` is defined in vulkan_video_codec_h264std.h
    // @see https://github.com/nvpro-samples/vk_video_samples/blob/c851b02743574def866c593fe66d1dff93354e6d/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h
    .stdProfileIdc = std_video_h264_profile_idc_high,
    .fieldLayout = VK_VIDEO_DECODE_H264_FIELD_LAYOUT_LINE_INTERLACED_PLANE_BIT_EXT,
  };
  decode_h264_profile_ = decode_h264_profile;
  video_profile.pNext = &decode_h264_profile_;
  //
  VkVideoCapabilitiesKHR video_capabilities = { VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, nullptr };
  vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(instance->Handle())(
    physical_device->Handle(), &video_profile, &video_capabilities);
  profile_with_ext_ = video_profile;
  profile_ = video_profile;
  profile_.pNext = nullptr;
}

VideoProfile::~VideoProfile() {
}

}  // namespace vk
