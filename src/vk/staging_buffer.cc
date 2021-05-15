
#include "vk/staging_buffer.h"

#include <iostream>
#include <cstring>
#include <algorithm>

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/device_queue.h"
#include "vk/buffer_memory.h"
#include "vk/command_pool.h"
#include "vk/command_buffer.h"
#include "vk/command_record.h"

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
    : command_pool_(command_pool),
      size_(size),
      allocation_size_(std::max(size, (VkDeviceSize)0x100)) {
  host_buffer_memory_ = vk::BufferMemory::Create(
    command_pool->DeviceQueue(),
    allocation_size_,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT).value();
  device_buffer_memory_ = vk::BufferMemory::Create(
    command_pool->DeviceQueue(),
    allocation_size_,
    buffer_usage_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT).value();
}

void StagingBuffer::Initialize() {
  auto command_record = vk::CommandRecord::Begin(command_pool_).value();
  VkBufferCopy copy = {};
  copy.size = size_;
  vkCmdCopyBuffer(command_record->CommandBuffer(),
    host_buffer_memory_->Buffer(),
    device_buffer_memory_->Buffer(),
    1,
    &copy);
  copy_command_buffer_ = command_record->End();
}

void StagingBuffer::Write(void *data, size_t size) {
  if (size != size_) {
    std::cout << "WARNING: mismatched size at StagingBuffer::Write()" << std::endl;
    return;
  }
  auto& device = command_pool_->DeviceQueue()->Device();
  {
    void *mapped = nullptr;
    vkMapMemory(device->Handle(), host_buffer_memory_->Memory(), 0, size_, 0, &mapped);
    std::memcpy(mapped, data, size);
    vkUnmapMemory(device->Handle(), host_buffer_memory_->Memory());
  }
  auto& queue = command_pool_->DeviceQueue()->Queue();
  queue->SubmitThenWait(copy_command_buffer_);
}

StagingBuffer::~StagingBuffer() {
}

}  // namespace vk
