
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
#include "render.h"

extern "C" {
#include <stdio.h>
}

#include <vector>
#include <string>
#include <optional>

#include "instance.h"
#include "physical_device.h"
#include "device_queue.h"
#include "device.h"

#include "video_demux.h"
#include "video_decode_session.h"

Render::Render() {
  auto instance = vk::Instance::Create(); 
  auto physical_device = vk::PhysicalDevice::Create(instance);
  auto device_queue = vk::DeviceQueue::Create(physical_device).value();
  auto device = device_queue->Device();
  auto queue = device_queue->Queue();

  auto session = std::make_unique<vk::VideoDecodeSession>(device_queue);
  session->Initialize();
}
