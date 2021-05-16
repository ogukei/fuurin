
#include "vk/command_buffer.h"

#include "vk/command_pool.h"
#include "vk/device_queue.h"
#include "vk/device.h"

namespace vk {

std::optional<std::shared_ptr<CommandBuffer>> CommandBuffer::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool) {
  auto command_buffer = std::make_shared<CommandBuffer>(command_pool);
  command_buffer->Initialize();
  return command_buffer;
}

CommandBuffer::CommandBuffer(
    const std::shared_ptr<vk::CommandPool>& command_pool)
    : command_pool_(command_pool),
      command_buffer_(nullptr) {
}

void CommandBuffer::Initialize() {
  auto& device = command_pool_->Device();
  VkCommandBufferAllocateInfo allocate_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = nullptr,
    .commandPool = command_pool_->Handle(),
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };
  VkCommandBuffer command_buffer = nullptr;
  vkAllocateCommandBuffers(device->Handle(), &allocate_info, &command_buffer);
  command_buffer_ = command_buffer;
}

CommandBuffer::~CommandBuffer() {
  auto& device = command_pool_->Device();
  vkFreeCommandBuffers(device->Handle(), command_pool_->Handle(), 1, &command_buffer_);
  command_buffer_ = nullptr;
}

}  // namespace vk
