
#include "vk/video_image_memory.h"

#include <iostream>

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"
#include "vk/video_image.h"

namespace vk {

std::shared_ptr<VideoImageMemory> VideoImageMemory::Create(
    const std::shared_ptr<vk::VideoImage>& video_image) {
  auto video_memory = std::make_shared<VideoImageMemory>(video_image);
  video_memory->Initialize();
  return video_memory;
}

VideoImageMemory::VideoImageMemory(
    const std::shared_ptr<vk::VideoImage>& video_image)
    : video_image_(video_image),
      memory_(nullptr) {
}

void VideoImageMemory::Initialize() {
  auto& device = video_image_->Device();
  auto& physical_device = device->PhysicalDevice();
  // requirements
  VkMemoryRequirements memory_requirements = {};
  vkGetImageMemoryRequirements(device->Handle(), video_image_->Handle(), &memory_requirements);
  // memory type index
  uint32_t memory_type_index = vk::MemoryTypeIndex(
      physical_device,
      memory_requirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    .value();
  // dedicated
  VkMemoryDedicatedAllocateInfo dedicated_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
    .pNext = nullptr,
    .image = video_image_->Handle(),
    .buffer = nullptr
  };
  // allocation
  VkMemoryAllocateInfo allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = &dedicated_allocate_info,
    .allocationSize = memory_requirements.size,
    .memoryTypeIndex = memory_type_index,
  };
  VkDeviceMemory memory = nullptr;
  vkAllocateMemory(device->Handle(), &allocate_info, nullptr, &memory);
  memory_ = memory;
  // bind
  vkBindImageMemory(device->Handle(), video_image_->Handle(), memory, 0);
}

VideoImageMemory::~VideoImageMemory() {
  auto& device = video_image_->Device();
  vkFreeMemory(device->Handle(), memory_, nullptr);
  memory_ = nullptr;
}

}  // namespace vk
