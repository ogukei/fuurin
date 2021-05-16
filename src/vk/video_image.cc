
#include "vk/video_image.h"

#include <iostream>

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/device_queue.h"
#include "vk/video_profile.h"

namespace vk {

std::shared_ptr<VideoImage> VideoImage::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format) {
  auto video_image = std::make_shared<VideoImage>(
    device, queue, video_profile, width, height, format);
  video_image->Initialize();
  return video_image;
}

VideoImage::VideoImage(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format)
    : device_(device),
      queue_(queue),
      video_profile_(video_profile),
      width_(width),
      height_(height),
      format_(format),
      image_(nullptr) {
}

void VideoImage::Initialize() {
  const VkVideoProfileKHR* video_profile = &video_profile_->Profile();
  VkExternalMemoryImageCreateInfo external_memory = {
    .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
    .pNext = video_profile,
    .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT,
  };
  uint32_t queue_family_index = queue_->FamilyIndex();
  VkImageCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = &external_memory,
    .flags = 0,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = format_,
    .extent = {
      .width = width_,
      .height = height_,
      .depth = 1,
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = VK_IMAGE_USAGE_SAMPLED_BIT
      | VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR
      | VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR
      | VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = &queue_family_index,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VkImage image = nullptr;
  vkCreateImage(device_->Handle(), &create_info, nullptr, &image);
  image_ = image;
}

VideoImage::~VideoImage() {
  vkDestroyImage(device_->Handle(), image_, nullptr);
  image_ = nullptr;
}

}  // namespace vk
