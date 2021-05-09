
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class CommandPool;
class BufferMemory;

class StagingBuffer {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::BufferMemory> host_buffer_memory_;
  std::shared_ptr<vk::BufferMemory> device_buffer_memory_;

  void Initialize();
 public:
  static std::optional<std::shared_ptr<StagingBuffer>> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    VkDeviceSize size,
    VkBufferUsageFlags buffer_usage_flags);

  explicit StagingBuffer(const std::shared_ptr<vk::CommandPool>& command_pool,
    VkDeviceSize size,
    VkBufferUsageFlags buffer_usage_flags);
  StagingBuffer(const StagingBuffer&) = delete;
  ~StagingBuffer();
};

}  // namespace vk
