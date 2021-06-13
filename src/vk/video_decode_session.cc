
#include "vk/video_decode_session.h"

#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

#include "vk/instance.h"
#include "vk/physical_device.h"
#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/queue.h"
#include "vk/command_pool.h"
#include "vk/command_record.h"
#include "vk/image_view.h"

#include "vk/video_profile.h"
#include "vk/video_memories.h"
#include "vk/video_frame.h"
#include "vk/video_image.h"
#include "vk/video_reference_slot.h"
#include "vk/video_bitstream.h"
#include "vk/video_session_parameters.h"
#include "vk/video_decode_surface.h"
#include "vk/video_reference_state.h"
#include "vk/video_query_pool.h"
#include "vk/video_offscreen_render.h"

#include "vk/video_h264_picture_info.h"
#include "vk/video_h264_picture_parameters.h"

#include "video/demux.h"

namespace vk {

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateVideoSessionKHR.html
static PFN_vkCreateVideoSessionKHR vk_vkCreateVideoSessionKHR(VkDevice device) {
    return (PFN_vkCreateVideoSessionKHR)vkGetDeviceProcAddr(device, "vkCreateVideoSessionKHR");
}

static PFN_vkDestroyVideoSessionKHR vk_vkDestroyVideoSessionKHR(VkDevice device) {
    return (PFN_vkDestroyVideoSessionKHR)vkGetDeviceProcAddr(device, "vkDestroyVideoSessionKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCmdBeginVideoCodingKHR.html
static PFN_vkCmdBeginVideoCodingKHR vk_vkCmdBeginVideoCodingKHR(VkDevice device) {
    return (PFN_vkCmdBeginVideoCodingKHR)vkGetDeviceProcAddr(device, "vkCmdBeginVideoCodingKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCmdDecodeVideoKHR.html
static PFN_vkCmdDecodeVideoKHR vk_vkCmdDecodeVideoKHR(VkDevice device) {
    return (PFN_vkCmdDecodeVideoKHR)vkGetDeviceProcAddr(device, "vkCmdDecodeVideoKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCmdEndVideoCodingKHR.html
static PFN_vkCmdEndVideoCodingKHR vk_vkCmdEndVideoCodingKHR(VkDevice device) {
    return (PFN_vkCmdEndVideoCodingKHR)vkGetDeviceProcAddr(device, "vkCmdEndVideoCodingKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCmdPipelineBarrier2KHR.html
static PFN_vkCmdPipelineBarrier2KHR vk_vkCmdPipelineBarrier2KHR(VkDevice device) {
    return (PFN_vkCmdPipelineBarrier2KHR)vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier2KHR");
}

std::optional<std::unique_ptr<VideoDecodeSession>> VideoDecodeSession::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::unique_ptr<video::Demux>& demux,
    const std::shared_ptr<vk::VideoBitstreamBuffer>& bitstream_buffer,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters) {
  auto session = std::make_unique<VideoDecodeSession>(command_pool, bitstream_buffer, picture_parameters);
  return (session->Initialize(demux)) ? std::optional {std::move(session)} : std::nullopt;
}

VideoDecodeSession::VideoDecodeSession(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::VideoBitstreamBuffer>& bitstream_buffer,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters)
    : command_pool_(command_pool),
      bitstream_buffer_(bitstream_buffer),
      picture_parameters_(picture_parameters),
      video_session_(nullptr) {
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#video-decode-operations
bool VideoDecodeSession::Initialize(const std::unique_ptr<video::Demux>& demux) {
  // FIXME:
  // assumes VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR
  // assumes VK_FORMAT_G8_B8R8_2PLANE_420_UNORM
  // assumes VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR

  auto& device = command_pool_->Device();
  auto& queue = command_pool_->Queue();
  auto& physical_device = device->PhysicalDevice();
  auto& instance = physical_device->Instance();

  auto video_profile = VideoProfile::CreateH264Decode(device);
  VkFormat format = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
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
    .maxReferencePicturesSlotsCount = 4,
    .maxReferencePicturesActiveCount = 4
  };
  VkVideoSessionKHR video_session = nullptr;
  vk_vkCreateVideoSessionKHR(device->Handle())(device->Handle(), &session_create_info, nullptr, &video_session);
  if (video_session == nullptr) {
    return false;
  }
  video_session_ = video_session;
  memories_ = VideoSessionMemories::Create(device, video_session);
  parameters_ = VideoSessionParameters::Create(device, picture_parameters_, video_session);
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/bbb10b1f34bbbff27b9f303cae4e287a9a676a3f/vk_video_decoder/libs/VkVideoParser/VulkanVideoParser.cpp#L1699
  // decode surfaces
  uint32_t num_decode_surface = 4;
  {
    decode_surfaces_.resize(num_decode_surface);
    for (uint32_t i = 0; i < num_decode_surface; i++) {
      auto frame = VideoSessionFrame::Create(device, queue, video_profile, demux->Width(), demux->Height(), format);
      auto decode_surface = VideoDecodeSurface::Create(frame, i);
      decode_surfaces_[i] = std::move(decode_surface);
    }
  }
  // dpb
  VkExtent2D extent = {
    demux->Width(), demux->Height()
  };
  reference_state_ = VideoReferenceState::Create(device, queue, video_profile, picture_parameters_, extent, format);
  video_query_pool_ = VideoQueryPool::Create(device, video_profile, num_decode_surface).value();
  // extent
  extent_ = VkExtent2D {
    .width = demux->Width(),
    .height = demux->Height()
  };
  return true;
}

void VideoDecodeSession::Begin(const std::shared_ptr<vk::H264PictureInfo>& picture_info) {
  auto segment_reference = bitstream_buffer_->PopFrontSegmentReference().value();
  auto command_record = CommandRecord::Begin(command_pool_).value();
  auto& device = command_pool_->Device();

  reference_state_->BeginDecode(picture_info);
  auto& image_barrier_vec = reference_state_->ImageMemoryBarriers();
  // prepare query
  vkCmdResetQueryPool(command_record->CommandBuffer(), video_query_pool_->Handle(), 0, video_query_pool_->QueryCount());
  // begin
  auto& reference_slots = reference_state_->ReferenceSlotsVec();
  VkVideoBeginCodingInfoKHR begin_coding_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_BEGIN_CODING_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
    .codecQualityPreset = VK_VIDEO_CODING_QUALITY_PRESET_NORMAL_BIT_KHR,
    .videoSession = video_session_,
    .videoSessionParameters = parameters_->Handle(),
    .referenceSlotCount = static_cast<uint32_t>(reference_slots.size()),
    .pReferenceSlots = reference_slots.data(),
  };
  vk_vkCmdBeginVideoCodingKHR(device->Handle())(command_record->CommandBuffer(), &begin_coding_info);
  {
    VkBufferMemoryBarrier2KHR bitstream_buffer_memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR,
      .pNext = nullptr,
      .srcStageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
      .srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT_KHR,
      .dstStageMask = VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR,
      .dstAccessMask = VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = command_pool_->Queue()->FamilyIndex(),
      .buffer = bitstream_buffer_->Buffer(),
      .offset = segment_reference.offset,
      .size = segment_reference.size,
    };
    VkDependencyInfoKHR dependency_info = {
      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
      .pNext = nullptr,
      .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
      .memoryBarrierCount = 0,
      .pMemoryBarriers = nullptr,
      .bufferMemoryBarrierCount = 1,
      .pBufferMemoryBarriers = &bitstream_buffer_memory_barrier,
      .imageMemoryBarrierCount = static_cast<uint32_t>(image_barrier_vec.size()),
      .pImageMemoryBarriers = image_barrier_vec.data(),
    };
    vk_vkCmdPipelineBarrier2KHR(device->Handle())(command_record->CommandBuffer(), &dependency_info);
  }
  //
  vkCmdBeginQuery(command_record->CommandBuffer(), video_query_pool_->Handle(), 0, 0);
  //
  VkVideoDecodeH264PictureInfoEXT decode_picture_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PICTURE_INFO_EXT,
    .pNext = nullptr,
    .pStdPictureInfo = picture_info->StdPictureInfo(),
    .slicesCount = picture_info->SlicesCount(),
    .pSlicesDataOffsets = picture_info->SlicesDataOffsets(),
  };
  VkVideoDecodeInfoKHR decode_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_INFO_KHR,
    .pNext = &decode_picture_info,
    .flags = 0,
    .codedOffset = {},
    .codedExtent = extent_,
    .srcBuffer = bitstream_buffer_->Buffer(),
    .srcBufferOffset = segment_reference.offset,
    .srcBufferRange = segment_reference.size,
    .dstPictureResource = reference_state_->PictureResourceDestination(),
    .pSetupReferenceSlot = &reference_state_->SetupReferenceSlotInfo(),
    .referenceSlotCount = static_cast<uint32_t>(reference_slots.size()),
    .pReferenceSlots = reference_slots.data(),
  };
  vk_vkCmdDecodeVideoKHR(device->Handle())(command_record->CommandBuffer(), &decode_info);
  //
  vkCmdEndQuery(command_record->CommandBuffer(), video_query_pool_->Handle(), 0);
  // end
  VkVideoEndCodingInfoKHR end_coding_info = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_END_CODING_INFO_KHR,
    .pNext = nullptr,
    .flags = 0,
  };
  vk_vkCmdEndVideoCodingKHR(device->Handle())(command_record->CommandBuffer(), &end_coding_info);

  auto command_buffer = command_record->End();
  command_pool_->Queue()->SubmitThenWaitStage(
    command_buffer,
    VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR);

  // query result
  video_query_pool_->DumpResult();
  //
  reference_state_->EndDecode(picture_info);
}

void VideoDecodeSession::DumpPicture(const std::string& filename) {
  // FIXME(ogukei): use decode surface pool to allow async outputs
  auto& slot = reference_state_->SetupReferenceSlot();
  auto& frame = slot->Frame();
  auto offscreen_render = vk::VideoOffscreenRender::Create(command_pool_, frame);
  offscreen_render->Execute();
  offscreen_render->Save(filename);
}

VideoDecodeSession::~VideoDecodeSession() {
  auto& device = command_pool_->Device();
  vk_vkDestroyVideoSessionKHR(device->Handle())(device->Handle(), video_session_, nullptr);
  video_session_ = nullptr;
}

}  // namespace vk
