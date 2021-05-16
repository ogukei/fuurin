
#include "vk/graphics_state.h"

#include <vector>

#include "vk/staging_buffer.h"

namespace vk {

std::shared_ptr<GraphicsState> GraphicsState::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool) {
  auto instance = std::make_shared<GraphicsState>(command_pool);
  instance->Initialize();
  return instance;
}

GraphicsState::GraphicsState(
    const std::shared_ptr<vk::CommandPool>& command_pool)
    : command_pool_(command_pool) {
}


void GraphicsState::Initialize() {
  // vertices
  std::vector<float> vertices = {
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };
  size_t vertices_size = vertices.size() * sizeof(float);
  auto vertices_staging_buffer = StagingBuffer::Create(
    command_pool_, vertices_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).value();
  vertices_staging_buffer->Write(vertices.data(), vertices_size);
  vertex_staging_buffer_ = vertices_staging_buffer;
  // indices
  std::vector<uint32_t> indices = {0, 1, 2};
  size_t indices_size = indices.size() * sizeof(uint32_t);
  auto indices_staging_buffer = StagingBuffer::Create(
    command_pool_, indices_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT).value();
  indices_staging_buffer->Write(indices.data(), indices_size);
  index_staging_buffer_ = indices_staging_buffer;
}

VkBuffer GraphicsState::VertexBuffer() const {
  return vertex_staging_buffer_->DeviceBuffer();
}

VkBuffer GraphicsState::IndexBuffer() const {
  return index_staging_buffer_->DeviceBuffer();
}

GraphicsState::~GraphicsState() {
}

}  // namespace vk
