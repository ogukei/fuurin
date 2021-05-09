
#include "vk/buffer_memory.h"

#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/physical_device.h"

namespace vk {

std::optional<uint32_t> MemoryTypeIndex(
    const std::shared_ptr<vk::PhysicalDevice> physical_device,
    uint32_t memory_type_bits,
    VkMemoryPropertyFlags property_flags) {
  VkPhysicalDeviceMemoryProperties properties = {};
  vkGetPhysicalDeviceMemoryProperties(physical_device->Handle(), &properties);
  for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
    if ((memory_type_bits & 1) == 1) {
      if ((properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
        return i;
      }
    }
    memory_type_bits >>= 1;
  }
  return std::nullopt;
}

std::optional<std::shared_ptr<BufferMemory>> BufferMemory::Create(
    const std::shared_ptr<vk::DeviceQueue>& device_queue,
    VkDeviceSize size,
    VkBufferUsageFlags buffer_usage_flags,
    VkMemoryPropertyFlags memory_property_flags) {
  auto buffer_memory = std::make_shared<BufferMemory>(
    device_queue, size, buffer_usage_flags, memory_property_flags);
  return (buffer_memory->Initialize()) ? std::optional {buffer_memory} : std::nullopt;
}

BufferMemory::BufferMemory(
    const std::shared_ptr<vk::DeviceQueue>& device_queue,
    VkDeviceSize size,
    VkBufferUsageFlags buffer_usage_flags,
    VkMemoryPropertyFlags memory_property_flags)
    : device_queue_(device_queue),
      buffer_(nullptr),
      size_(size),
      buffer_usage_flags_(buffer_usage_flags),
      memory_property_flags_(memory_property_flags) {
}

bool BufferMemory::Initialize() {
  auto& device = device_queue_->Device();
  uint32_t queue_family_index = device_queue_->QueueFamilyIndex();
  // buffer
  VkBuffer buffer = nullptr;
  {
    VkBufferCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = size_,
      .usage = buffer_usage_flags_,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &queue_family_index
    };
    vkCreateBuffer(device->Handle(), &create_info, nullptr, &buffer);
  }
  if (buffer == nullptr) {
    return false;
  }
  buffer_ = buffer;
  // memory
  VkDeviceMemory memory = nullptr;
  {
    VkMemoryRequirements requirements = {};
    vkGetBufferMemoryRequirements(device->Handle(), buffer, &requirements);
    auto memory_type_index = vk::MemoryTypeIndex(
      device_queue_->PhysicalDevice(),
      requirements.memoryTypeBits,
      memory_property_flags_);
    if (!memory_type_index) {
      return false;
    }
    VkMemoryAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = size_,
      .memoryTypeIndex = memory_type_index.value(),
    };
    vkAllocateMemory(device->Handle(), &allocate_info, nullptr, &memory);
  }
  if (memory == nullptr) {
    return false;
  }
  memory_ = memory;
  // binding
  vkBindBufferMemory(device->Handle(), buffer, memory, 0);
  return true;
}

BufferMemory::~BufferMemory() {
  auto& device = device_queue_->Device();
  vkFreeMemory(device->Handle(), memory_, nullptr);
  memory_ = nullptr;
  vkDestroyBuffer(device->Handle(), buffer_, nullptr);
  buffer_ = nullptr;
}


}  // namespace vk
