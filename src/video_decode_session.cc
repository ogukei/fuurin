
#include "video_decode_session.h"

namespace vk {

#define VK_MAKE_VIDEO_STD_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

// @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h#L5
#define VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9 VK_MAKE_VIDEO_STD_VERSION(0, 9, 0) // Patch version should always be set to 0

// Format must be in the form XX.XX where the first two digits are the major and the second two, the minor.
#define VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION   VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9
#define VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME "VK_STD_vulkan_video_codec_h264"

static PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(VkInstance instance) {
  return (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
}

VideoDecodeSession::VideoDecodeSession() {

}

void VideoDecodeSession::Initialize() {

  // // @see PopulateProfileExt 
  // // https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/nvVideoProfile.h#L55
  // VkVideoProfileKHR video_profile = {
  //   .sType = VK_STRUCTURE_TYPE_VIDEO_PROFILE_KHR,
  //   .pNext = nullptr,
  //   .videoCodecOperation = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT,
  //   // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoChromaSubsamplingFlagBitsKHR.html
  //   .chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
  //   // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoComponentBitDepthFlagBitsKHR.html
  //   .lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
  //   .chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
  // };
  // // @see https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/nvVideoProfile.h#L55
  // VkVideoDecodeH264ProfileEXT decode_h264_profile = {
  //   .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_EXT,
  //   .pNext = nullptr,
  //   // note that `std_video_h264_profile_idc_main` is defined in vulkan_video_codec_h264std.h
  //   // @see https://github.com/nvpro-samples/vk_video_samples/blob/c851b02743574def866c593fe66d1dff93354e6d/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h
  //   .stdProfileIdc = std_video_h264_profile_idc_high,
  //   .fieldLayout = VK_VIDEO_DECODE_H264_FIELD_LAYOUT_LINE_INTERLACED_PLANE_BIT_EXT,
  // };
  // video_profile.pNext = &decode_h264_profile;
  // VkVideoCapabilitiesKHR video_capabilities = { VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, nullptr };
  // vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(instance_->Handle())(physical_device_->Handle(), &video_profile, &video_capabilities);
  
  // // @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/libs/NvVkDecoder/NvVkDecoder.cpp#L699
  // static const VkExtensionProperties h264StdExtensionVersion = { 
  //   VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME, 
  //   VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION
  // };
  // VkVideoDecodeH264SessionCreateInfoEXT decode_session_create_info = {
  //   .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_SESSION_CREATE_INFO_EXT,
  //   .pNext = nullptr,
  //   .flags = 0,
  //   .pStdExtensionVersion = &h264StdExtensionVersion,
  // };
  // VkVideoSessionCreateInfoKHR session_create_info = {
  //   .sType = VK_STRUCTURE_TYPE_VIDEO_SESSION_CREATE_INFO_KHR,
  //   .pNext = &decode_session_create_info,
  //   .flags = 0,
  //   .pVideoProfile = &video_profile,
  //   .maxCodedExtent = {

  //   },
  // };
}

VideoDecodeSession::~VideoDecodeSession() {

}

} // namespace vk
