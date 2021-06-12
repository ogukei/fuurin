
#include "app/render.h"

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

#include <unistd.h>
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
#include "vk/video_bitstream.h"
#include "vk/video_h264_picture_info.h"

#include "video/demux.h"
#include "video/nvidia_video_parser.h"

Render::Render() {
  auto instance = vk::Instance::Create();
  auto physical_device = vk::PhysicalDevice::Create(instance);
  auto device_queue = vk::DeviceQueue::Create(physical_device).value();
  auto device = device_queue->Device();
  auto graphics_command_pool = vk::CommandPool::Create(device, device_queue->GraphicsQueue()).value();

  auto framebuffer = vk::Framebuffer::Create(device, 1280, 720);
  auto graphics_pipeline = vk::GraphicsPipeline::Create(device, framebuffer);
  auto graphics_state = vk::GraphicsState::Create(graphics_command_pool);
  auto graphics_render = vk::GraphicsRender::Create(graphics_command_pool, graphics_pipeline, graphics_state);
  // graphics_render->Execute();

  // auto offscreen_render = vk::OffscreenRender::Create(graphics_command_pool, framebuffer);
  // offscreen_render->Execute();
  // offscreen_render->Save("out.ppm");

  auto demux = video::CreateDemux("/home/user/Downloads/BigBuckBunny.mp4");
  auto video_command_pool = vk::CommandPool::Create(device, device_queue->VideoQueue()).value();
  auto bitstream_buffer = vk::VideoBitstreamBuffer::Create(video_command_pool);

  auto parser = std::make_unique<video::NvidiaVideoParser>();
  for (uint32_t i = 0; i < 10; i++) {
    auto packet = demux->NextPacket().value();
    parser->Parse(packet);
    if (parser->IsSequenceReady()) {
      break;
    }
  }
  auto session = vk::VideoDecodeSession::Create(
    video_command_pool,
    demux,
    bitstream_buffer,
    parser->PictureParameters()).value();
  // register callbacks
  parser->RegisterCallback([&](const std::shared_ptr<vk::H264PictureInfo>& picture_info) {
    bitstream_buffer->AppendSegment(picture_info->BitstreamSegment());
    session->Begin(picture_info);
  });
  for (uint32_t i = 0; i < 1; i++) {
    auto packet = demux->NextPacket().value();
    parser->Parse(packet);
  }
}
