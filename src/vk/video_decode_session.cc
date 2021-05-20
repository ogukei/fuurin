
#include "vk/video_decode_session.h"

#include <iostream>
#include <vector>
#include <utility>

#include "vk/instance.h"
#include "vk/physical_device.h"
#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/queue.h"
#include "vk/command_pool.h"
#include "vk/command_record.h"

#include "vk/video_profile.h"
#include "vk/video_memories.h"
#include "vk/video_frame.h"
#include "vk/video_reference_slot.h"
#include "vk/video_bitstream.h"
#include "vk/video_session_parameters.h"

#include "video/demux.h"

namespace vk {

#define VK_MAKE_VIDEO_STD_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

// @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h#L5
// Patch version should always be set to 0
#define VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9 VK_MAKE_VIDEO_STD_VERSION(0, 9, 0)

// Format must be in the form XX.XX where the first two digits are the major and the second two, the minor.
#define VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION   VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9
#define VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME "VK_STD_vulkan_video_codec_h264"

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateVideoSessionKHR.html
static PFN_vkCreateVideoSessionKHR vk_vkCreateVideoSessionKHR(VkDevice device) {
    return (PFN_vkCreateVideoSessionKHR)vkGetDeviceProcAddr(device, "vkCreateVideoSessionKHR");
}

static PFN_vkDestroyVideoSessionKHR vk_vkDestroyVideoSessionKHR(VkDevice device) {
    return (PFN_vkDestroyVideoSessionKHR)vkGetDeviceProcAddr(device, "vkDestroyVideoSessionKHR");
}

VideoDecodeSession::VideoDecodeSession(const std::shared_ptr<vk::CommandPool>& command_pool)
    : command_pool_(command_pool) {
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#video-decode-operations
bool VideoDecodeSession::Initialize(const std::unique_ptr<video::Demux>& demux) {
  // FIXME:
  // assumes VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR
  // assumes VK_FORMAT_G8_B8R8_2PLANE_420_UNORM
  // assumes VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR
  // assumes VK_VIDEO_DECODE_H264_FIELD_LAYOUT_LINE_INTERLACED_PLANE_BIT_EXT

  auto& device = command_pool_->Device();
  auto& queue = command_pool_->Queue();
  auto& physical_device = device->PhysicalDevice();
  auto& instance = physical_device->Instance();

  auto video_profile = VideoProfile::CreateH264Decode(device);
  // isSemiPlanar == true
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/95eeeb80879e04183923e2be3d0b93b3652ab868/vk_video_decoder/libs/NvVkDecoder/NvVkDecoder.cpp#L131
  VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
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
    .queueFamilyIndex = queue->FamilyIndex(),
    .flags = 0,
    .pVideoProfile = &video_profile->ProfileWithExt(),
    .pictureFormat = format,
    .maxCodedExtent = VkExtent2D {
      .width = demux->Width(),
      .height = demux->Height()
    },
    .referencePicturesFormat = format,
    .maxReferencePicturesSlotsCount = 17,
    .maxReferencePicturesActiveCount = 17
  };
  VkVideoSessionKHR video_session = nullptr;
  vk_vkCreateVideoSessionKHR(device->Handle())(device->Handle(), &session_create_info, nullptr, &video_session);
  if (video_session == nullptr) {
    return false;
  }
  video_session_ = video_session;
  memories_ = VideoSessionMemories::Create(device, video_session);
  // video session parameters cannot be built at the moment
  // parameters_ = VideoSessionParameters::Create(device, video_session);
  bitstream_buffer_ = VideoBitstreamBuffer::Create(command_pool_);
  // slots
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/bbb10b1f34bbbff27b9f303cae4e287a9a676a3f/vk_video_decoder/libs/VkVideoParser/VulkanVideoParser.cpp#L1699
  uint32_t slot_count = 17;
  slots_.resize(slot_count);
  for (uint32_t i = 0; i < slot_count; i++) {
    auto frame = VideoSessionFrame::Create(device, queue, video_profile, demux->Width(), demux->Height(), format);
    auto slot = VideoReferenceSlot::Create(frame, i);
    slots_[i] = std::move(slot);
  }
  return true;
}

void VideoDecodeSession::Begin() {
  auto segment_reference = bitstream_buffer_->PrependSegmentReference();
  auto command_record = CommandRecord::Begin(command_pool_).value();
  VkVideoBeginCodingInfoKHR begin_coding_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_BEGIN_CODING_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .codecQualityPreset = VK_VIDEO_CODING_QUALITY_PRESET_NORMAL_BIT_KHR,
    .videoSession = video_session_,
    .videoSessionParameters = nullptr,
    .referenceSlotCount = static_cast<uint32_t>(slots_.size()),
    .pReferenceSlots = nullptr,
  };
}

VideoDecodeSession::~VideoDecodeSession() {
  auto& device = command_pool_->Device();
  vk_vkDestroyVideoSessionKHR(device->Handle())(device->Handle(), video_session_, nullptr);
  video_session_ = nullptr;
}

}  // namespace vk
