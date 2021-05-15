
#include "vk/image_memory.h"

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"

namespace vk {

std::shared_ptr<ImageMemory> ImageMemory::Create(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkMemoryPropertyFlags memory_property_flags) {
  auto image_memory = std::make_shared<ImageMemory>(device, image, memory_property_flags);
  image_memory->Initialize();
  return image_memory;
}

ImageMemory::ImageMemory(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkMemoryPropertyFlags memory_property_flags)
    : image_(image),
      device_(device),
      memory_(nullptr),
      memory_property_flags_(memory_property_flags) {
}

void ImageMemory::Initialize() {
  auto& physical_device = device_->DeviceQueue()->PhysicalDevice();
  // requirements
  VkMemoryRequirements memory_requirements = {};
  vkGetImageMemoryRequirements(device_->Handle(), image_, &memory_requirements);
  // memory type index
  uint32_t memoryTypeIndex = vk::MemoryTypeIndex(
    physical_device,
    memory_requirements.memoryTypeBits,
    memory_property_flags_).value();
  // allocation
  VkDeviceSize allocation_size = memory_requirements.size;
  VkMemoryAllocateInfo allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .pNext = nullptr,
    .allocationSize = allocation_size,
    .memoryTypeIndex = memoryTypeIndex,
  };
  VkDeviceMemory memory = nullptr;
  vkAllocateMemory(device_->Handle(), &allocate_info, nullptr, &memory);
  memory_ = memory;
  // bind
  vkBindImageMemory(device_->Handle(), image_, memory, 0);
}

ImageMemory::~ImageMemory() {
  vkFreeMemory(device_->Handle(), memory_, nullptr);
  memory_ = nullptr;
}

}  // namespace vk
