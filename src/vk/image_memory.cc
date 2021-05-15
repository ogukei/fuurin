
#include "vk/image_memory.h"

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"

namespace vk {

std::shared_ptr<ImageMemory> ImageMemory::Create(VkImage image, const std::shared_ptr<vk::Device>& device) {
  auto instance = std::make_shared<ImageMemory>(image, device);
  instance->Initialize();
  return instance;
}

ImageMemory::ImageMemory(VkImage image, const std::shared_ptr<vk::Device>& device)
    : image_(image),
      device_(device),
      memory_(nullptr) {
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
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).value();
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
