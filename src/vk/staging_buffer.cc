
#include "vk/staging_buffer.h"

#include "vk/buffer_memory.h"
#include "vk/command_pool.h"

namespace vk {

std::optional<std::shared_ptr<StagingBuffer>> StagingBuffer::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    VkDeviceSize size,
    VkBufferUsageFlags buffer_usage_flags) {
  auto staging_buffer = std::make_shared<StagingBuffer>(command_pool, size, buffer_usage_flags);
  staging_buffer->Initialize();
  return staging_buffer;
}

StagingBuffer::StagingBuffer(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    VkDeviceSize size,
    VkBufferUsageFlags buffer_usage_flags)
    : command_pool_(command_pool) {
  host_buffer_memory_ = vk::BufferMemory::Create(
    command_pool->DeviceQueue(),
    size,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT).value();
  device_buffer_memory_ = vk::BufferMemory::Create(
    command_pool->DeviceQueue(),
    size,
    buffer_usage_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).value();
}

void StagingBuffer::Initialize() {

}

StagingBuffer::~StagingBuffer() {

}

}  // namespace vk
