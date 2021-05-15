
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class CommandPool;
class StagingBuffer;

class GraphicsState {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::StagingBuffer> vertex_staging_buffer_;
  std::shared_ptr<vk::StagingBuffer> index_staging_buffer_;

  void Initialize();
 public:
  static std::shared_ptr<GraphicsState> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  explicit GraphicsState(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  GraphicsState(const GraphicsState&) = delete;
  ~GraphicsState();

  VkBuffer VertexBuffer() const;
  VkBuffer IndexBuffer() const;
};

}  // namespace vk
