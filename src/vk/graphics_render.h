
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class CommandPool;
class GraphicsPipeline;
class GraphicsState;
class CommandBuffer;

class GraphicsRender {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::GraphicsPipeline> pipeline_;
  std::shared_ptr<vk::GraphicsState> state_;

  std::shared_ptr<vk::CommandBuffer> command_buffer_;

  void Initialize();
 public:
  static std::shared_ptr<GraphicsRender> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::GraphicsPipeline>& pipeline,
    const std::shared_ptr<vk::GraphicsState>& state);

  explicit GraphicsRender(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::GraphicsPipeline>& pipeline,
    const std::shared_ptr<vk::GraphicsState>& state);

  GraphicsRender(const GraphicsRender&) = delete;
  ~GraphicsRender();

  void Execute();
};

}  // namespace vk
