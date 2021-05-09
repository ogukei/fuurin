
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class CommandPool;
class CommandBuffer;

class CommandRecord {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::CommandBuffer> command_buffer_;
  bool is_oneshot_;

  void Initialize();
 public:
  static std::optional<std::shared_ptr<CommandRecord>> Begin(
    const std::shared_ptr<vk::CommandPool>& command_pool);
  static std::optional<std::shared_ptr<CommandRecord>> BeginOneshot(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  std::shared_ptr<vk::CommandBuffer> End();

  explicit CommandRecord(const std::shared_ptr<vk::CommandPool>& command_pool, bool is_oneshot);
  CommandRecord(const CommandRecord&) = delete;
  ~CommandRecord();
};

}  // namespace vk
