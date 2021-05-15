
#include "app/render.h"

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <iostream>

#include "vk/instance.h"
#include "vk/physical_device.h"
#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/command_pool.h"
#include "vk/staging_buffer.h"
#include "vk/framebuffer.h"
#include "vk/shader_module.h"
#include "vk/graphics_pipeline.h"

#include "video/video_demux.h"
#include "video/video_decode_session.h"

Render::Render() {
  auto instance = vk::Instance::Create();
  auto physical_device = vk::PhysicalDevice::Create(instance);
  auto device_queue = vk::DeviceQueue::Create(physical_device).value();
  auto device = device_queue->Device();
  auto queue = device_queue->Queue();
  auto command_pool = vk::CommandPool::Create(device_queue).value();

  auto session = std::make_unique<vk::VideoDecodeSession>(device_queue);
  session->Initialize();

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
  auto vertices_staging_buffer = vk::StagingBuffer::Create(
    command_pool, vertices_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).value();
  vertices_staging_buffer->Write(vertices.data(), vertices_size);
  // indices
  std::vector<uint32_t> indices = {0, 1, 2};
  size_t indices_size = indices.size() * sizeof(uint32_t);
  auto indices_staging_buffer = vk::StagingBuffer::Create(
    command_pool, indices_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT).value();
  indices_staging_buffer->Write(indices.data(), indices_size);

  auto framebuffer = vk::Framebuffer::Create(device, 1280, 720);
  auto graphics_pipeline = vk::GraphicsPipeline::Create(device, framebuffer);
}
