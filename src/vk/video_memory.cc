
#include "vk/video_memory.h"

#include <iostream>

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"

namespace vk {

std::shared_ptr<VideoSessionMemory> VideoSessionMemory::Create(
    const std::shared_ptr<vk::Device>& device,
    VkDeviceSize allocation_size,
    uint32_t memory_type_bits) {
  auto video_memory = std::make_shared<VideoSessionMemory>(device, allocation_size, memory_type_bits);
  video_memory->Initialize();
  return video_memory;
}

VideoSessionMemory::VideoSessionMemory(
    const std::shared_ptr<vk::Device>& device,
    VkDeviceSize allocation_size,
    uint32_t memory_type_bits)
    : device_(device),
      allocation_size_(allocation_size),
      memory_type_bits_(memory_type_bits),
      memory_(nullptr) {
}

void VideoSessionMemory::Initialize() {
  auto& physical_device = device_->PhysicalDevice();
  // memory type index
  // the sample app does not check this.
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/VulkanVideoUtils.cpp#L432
  // uint32_t memory_type_index = vk::MemoryTypeIndex(
  //   physical_device,
  //   memory_type_bits_,
  //   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).value();
  if (memory_type_bits_ == 0) return;
  uint32_t memory_type_index;
  uint32_t memory_type_bits = memory_type_bits_;
  for (memory_type_index = 0; !(memory_type_bits & 1); memory_type_index++) {
    memory_type_bits >>= 1;
  }
  // allocation
  VkMemoryAllocateInfo allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = nullptr,
    .allocationSize = allocation_size_,
    .memoryTypeIndex = memory_type_index,
  };
  VkDeviceMemory memory = nullptr;
  vkAllocateMemory(device_->Handle(), &allocate_info, nullptr, &memory);
  memory_ = memory;
}

VideoSessionMemory::~VideoSessionMemory() {
  vkFreeMemory(device_->Handle(), memory_, nullptr);
  memory_ = nullptr;
}

}  // namespace vk
