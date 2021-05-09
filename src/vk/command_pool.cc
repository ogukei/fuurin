
#include "vk/command_pool.h"

#include "vk/device_queue.h"
#include "vk/device.h"

namespace vk {

std::optional<std::shared_ptr<CommandPool>> CommandPool::Create(
    const std::shared_ptr<vk::DeviceQueue>& device_queue) {
  auto command_pool = std::make_shared<CommandPool>(device_queue);
  return (command_pool->Initialize()) ? std::optional {command_pool} : std::nullopt;
}

CommandPool::CommandPool(const std::shared_ptr<vk::DeviceQueue>& device_queue)
    : device_queue_(device_queue), command_pool_(nullptr) {
}

bool CommandPool::Initialize() {
  auto& device = device_queue_->Device();
  VkCommandPoolCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueFamilyIndex = device_queue_->QueueFamilyIndex(),
  };
  VkCommandPool command_pool = nullptr;
  vkCreateCommandPool(device->Handle(), &create_info, nullptr, &command_pool);
  if (command_pool == nullptr) return false;
  command_pool_ = command_pool;
  return true;
}

CommandPool::~CommandPool() {
  auto& device = device_queue_->Device();
  vkDestroyCommandPool(device->Handle(), command_pool_, nullptr);
  command_pool_ = nullptr;
}

}  // namespace vk
