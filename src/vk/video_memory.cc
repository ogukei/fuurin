
#include "vk/video_memory.h"

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"

namespace vk {

std::shared_ptr<VideoSessionMemory> VideoSessionMemory::Create(
    const std::shared_ptr<vk::Device>& device,
    VkDeviceSize allocation_size,
    uint32_t memory_type_bits,
    VkMemoryPropertyFlags memory_property_flags) {
  auto video_memory = std::make_shared<VideoSessionMemory>(device,
    allocation_size, memory_type_bits, memory_property_flags);
  video_memory->Initialize();
  return video_memory;
}

VideoSessionMemory::VideoSessionMemory(
    const std::shared_ptr<vk::Device>& device,
    VkDeviceSize allocation_size,
    uint32_t memory_type_bits,
    VkMemoryPropertyFlags memory_property_flags)
    : device_(device),
      allocation_size_(allocation_size),
      memory_type_bits_(memory_type_bits),
      memory_property_flags_(memory_property_flags),
      memory_(nullptr) {
}

void VideoSessionMemory::Initialize() {
  auto& physical_device = device_->DeviceQueue()->PhysicalDevice();
  // memory type index
  uint32_t memory_type_index = vk::MemoryTypeIndex(
    physical_device,
    memory_type_bits_,
    memory_property_flags_).value();
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
