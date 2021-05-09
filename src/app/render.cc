
#include "app/render.h"

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <vector>
#include <string>
#include <optional>
#include <memory>

#include "vk/instance.h"
#include "vk/physical_device.h"
#include "vk/device_queue.h"
#include "vk/device.h"

#include "video/video_demux.h"
#include "video/video_decode_session.h"

Render::Render() {
  auto instance = vk::Instance::Create();
  auto physical_device = vk::PhysicalDevice::Create(instance);
  auto device_queue = vk::DeviceQueue::Create(physical_device).value();
  auto device = device_queue->Device();
  auto queue = device_queue->Queue();

  auto session = std::make_unique<vk::VideoDecodeSession>(device_queue);
  session->Initialize();
}
