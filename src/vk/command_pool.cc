
#include "vk/command_pool.h"

#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/queue.h"

namespace vk {

std::optional<std::shared_ptr<CommandPool>> CommandPool::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue) {
  auto command_pool = std::make_shared<CommandPool>(device, queue);
  return (command_pool->Initialize()) ? std::optional {command_pool} : std::nullopt;
}

CommandPool::CommandPool(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue)
    : device_(device),
      queue_(queue),
      command_pool_(nullptr) {
}

bool CommandPool::Initialize() {
  auto& device = device_;
  VkCommandPoolCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueFamilyIndex = queue_->FamilyIndex(),
  };
  VkCommandPool command_pool = nullptr;
  vkCreateCommandPool(device->Handle(), &create_info, nullptr, &command_pool);
  if (command_pool == nullptr) return false;
  command_pool_ = command_pool;
  return true;
}

CommandPool::~CommandPool() {
  auto& device = device_;
  vkDestroyCommandPool(device->Handle(), command_pool_, nullptr);
  command_pool_ = nullptr;
}

}  // namespace vk
