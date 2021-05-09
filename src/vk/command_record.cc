
#include "vk/command_record.h"

#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/command_pool.h"
#include "vk/command_buffer.h"

namespace vk {

std::optional<std::shared_ptr<CommandRecord>> CommandRecord::Begin(
    const std::shared_ptr<vk::CommandPool>& command_pool) {
  auto record = std::make_shared<CommandRecord>(command_pool, false);
  record->Initialize();
  return record;
}

std::optional<std::shared_ptr<CommandRecord>> CommandRecord::BeginOnetimeSubmit(
    const std::shared_ptr<vk::CommandPool>& command_pool) {
  auto record = std::make_shared<CommandRecord>(command_pool, true);
  record->Initialize();
  return record;
}

CommandRecord::CommandRecord(
    const std::shared_ptr<vk::CommandPool>& command_pool, bool is_oneshot)
    : command_pool_(command_pool),
      is_oneshot_(is_oneshot) {
}

void CommandRecord::Initialize() {
  command_buffer_ = vk::CommandBuffer::Create(command_pool_).value();
  VkCommandBufferBeginInfo begin_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = (is_oneshot_) ? VkCommandBufferUsageFlags {VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT} : 0,
    .pInheritanceInfo = nullptr,
  };
  vkBeginCommandBuffer(command_buffer_->Handle(), &begin_info);
}

std::shared_ptr<vk::CommandBuffer> CommandRecord::End() {
  vkEndCommandBuffer(command_buffer_->Handle());
  return command_buffer_;
}

VkCommandBuffer CommandRecord::CommandBuffer() const {
  return command_buffer_->Handle();
}

CommandRecord::~CommandRecord() {
}

}  // namespace vk
