
#include "vk/video_decode_session.h"

#include <iostream>
#include <vector>
#include <utility>

#include "vk/device_queue.h"
#include "vk/physical_device.h"
#include "vk/instance.h"
#include "vk/device.h"

#include "vk/video_memories.h"

#include "video/video_demux.h"

namespace vk {

#define VK_MAKE_VIDEO_STD_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

// @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h#L5
// Patch version should always be set to 0
#define VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9 VK_MAKE_VIDEO_STD_VERSION(0, 9, 0)

// Format must be in the form XX.XX where the first two digits are the major and the second two, the minor.
#define VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION   VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9
#define VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME "VK_STD_vulkan_video_codec_h264"

static PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(VkInstance instance) {
  return (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)vkGetInstanceProcAddr(
    instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateVideoSessionKHR.html
static PFN_vkCreateVideoSessionKHR vk_vkCreateVideoSessionKHR(VkDevice device) {
    return (PFN_vkCreateVideoSessionKHR)vkGetDeviceProcAddr(device, "vkCreateVideoSessionKHR");
}

static PFN_vkDestroyVideoSessionKHR vk_vkDestroyVideoSessionKHR(VkDevice device) {
    return (PFN_vkDestroyVideoSessionKHR)vkGetDeviceProcAddr(device, "vkDestroyVideoSessionKHR");
}

VideoDecodeSession::VideoDecodeSession(const std::shared_ptr<vk::DeviceQueue>& device_queue)
    : device_queue_(device_queue) {
}

bool VideoDecodeSession::Initialize() {
  // assumes VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR
  // assumes VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM
  // assumes VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR
  // assumes VK_VIDEO_DECODE_H264_FIELD_LAYOUT_LINE_INTERLACED_PLANE_BIT_EXT
  auto demux = video::CreateDemux("/home/user/Downloads/BigBuckBunny.mp4");

  auto& physical_device = device_queue_->PhysicalDevice();
  auto& instance = physical_device->Instance();
  auto& device = device_queue_->Device();

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
  video_profile.pNext = &decode_h264_profile;
  //
  VkVideoCapabilitiesKHR video_capabilities = { VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, nullptr };
  vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(instance->Handle())(
    physical_device->Handle(), &video_profile, &video_capabilities);

  // @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/libs/NvVkDecoder/NvVkDecoder.cpp#L699
  static const VkExtensionProperties h264StdExtensionVersion = {
    VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME,
    VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION
  };
  VkVideoDecodeH264SessionCreateInfoEXT decode_session_create_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_SESSION_CREATE_INFO_EXT,
    .pNext = nullptr,
    .flags = 0,
    .pStdExtensionVersion = &h264StdExtensionVersion,
  };
  VkVideoSessionCreateInfoKHR session_create_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_SESSION_CREATE_INFO_KHR,
    .pNext = &decode_session_create_info,
    .queueFamilyIndex = device_queue_->QueueFamilyIndex(),
    .flags = 0,
    .pVideoProfile = &video_profile,
    .pictureFormat = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
    .maxCodedExtent = VkExtent2D {
      .width = demux->Width(),
      .height = demux->Height()
    },
    .referencePicturesFormat = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
    .maxReferencePicturesSlotsCount = 8,
    .maxReferencePicturesActiveCount = 8
  };
  VkVideoSessionKHR video_session = nullptr;
  vk_vkCreateVideoSessionKHR(device->Handle())(device->Handle(), &session_create_info, nullptr, &video_session);
  if (video_session == nullptr) {
    return false;
  }
  video_session_ = video_session;
  // FIXME: vulkan session should NOT have video demux logic
  video_demux_ = std::move(demux);
  memories_ = VideoSessionMemories::Create(device, video_session);
  return true;
}

VideoDecodeSession::~VideoDecodeSession() {
  auto& device = device_queue_->Device();
  vk_vkDestroyVideoSessionKHR(device->Handle())(device->Handle(), video_session_, nullptr);
  video_session_ = nullptr;
}

}  // namespace vk
