
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
#include "vk/graphics_state.h"
#include "vk/graphics_render.h"
#include "vk/offscreen_render.h"

#include "vk/video_decode_session.h"

#include "video/video_demux.h"

Render::Render() {
  auto instance = vk::Instance::Create();
  auto physical_device = vk::PhysicalDevice::Create(instance);
  auto device_queue = vk::DeviceQueue::Create(physical_device).value();
  auto device = device_queue->Device();
  auto command_pool = vk::CommandPool::Create(device, device_queue->GraphicsQueue()).value();

  auto framebuffer = vk::Framebuffer::Create(device, 1280, 720);
  auto graphics_pipeline = vk::GraphicsPipeline::Create(device, framebuffer);
  auto graphics_state = vk::GraphicsState::Create(command_pool);
  auto graphics_render = vk::GraphicsRender::Create(command_pool, graphics_pipeline, graphics_state);
  graphics_render->Execute();

  auto offscreen_render = vk::OffscreenRender::Create(command_pool, framebuffer);
  offscreen_render->Execute();
  offscreen_render->Save("out.ppm");

  auto session = std::make_unique<vk::VideoDecodeSession>(device_queue);
  session->Initialize();
}
