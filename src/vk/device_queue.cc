
#include "vk/device_queue.h"

#include <vector>
#include <iostream>

#include "vk/physical_device.h"
#include "vk/queue.h"
#include "vk/device.h"

namespace vk {

std::optional<std::shared_ptr<DeviceQueue>> DeviceQueue::Create(
    const std::shared_ptr<vk::PhysicalDevice>& physical_device) {
  auto device_queue = std::make_shared<DeviceQueue>(physical_device);
  return (device_queue->Initialize(device_queue)) ? std::optional {device_queue} : std::nullopt;
}

DeviceQueue::DeviceQueue(const std::shared_ptr<vk::PhysicalDevice>& physical_device)
    : physical_device_(physical_device) {
}

static std::optional<uint32_t> GraphicsQueueFamilyIndex(
    const std::shared_ptr<vk::PhysicalDevice>& physical_device) {
  uint32_t family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device->Handle(), &family_count, nullptr);
  std::vector<VkQueueFamilyProperties> families;
  families.resize(family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device->Handle(), &family_count, families.data());
  for (uint32_t i = 0; i < family_count; i++) {
    const VkQueueFamilyProperties& properties = families.at(i);
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      return i;
    }
  }
  return std::nullopt;
}

static std::optional<uint32_t> VideoQueueFamilyIndex(
    const std::shared_ptr<vk::PhysicalDevice>& physical_device) {
  VkVideoCodecOperationFlagsKHR video_codec_operations = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT
    | VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_EXT
    | VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_EXT;
  uint32_t family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties2(physical_device->Handle(), &family_count, nullptr);
  std::vector<VkQueueFamilyProperties2> families;
  std::vector<VkVideoQueueFamilyProperties2KHR> video_queues;
  families.resize(family_count);
  video_queues.resize(family_count);
  for (uint32_t i = 0; i < family_count; i++) {
    VkQueueFamilyProperties2* family = &families.at(i);
    VkVideoQueueFamilyProperties2KHR* video_queue = &video_queues.at(i);
    family->sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    video_queue->sType = VK_STRUCTURE_TYPE_VIDEO_QUEUE_FAMILY_PROPERTIES_2_KHR;
    video_queue->pNext = nullptr;
    // FIXME: this supresses wrong incorrect validation warnings. the validation implementation seems wrong
    video_queue->videoCodecOperations = 1;
    family->pNext = video_queue;
  }
  vkGetPhysicalDeviceQueueFamilyProperties2(physical_device->Handle(), &family_count, families.data());
  for (uint32_t i = 0; i < family_count; i++) {
    const VkQueueFamilyProperties& properties = families.at(i).queueFamilyProperties;
    const VkVideoQueueFamilyProperties2KHR* video_queue = &video_queues.at(i);
    if (properties.queueFlags & (VK_QUEUE_VIDEO_DECODE_BIT_KHR | VK_QUEUE_VIDEO_ENCODE_BIT_KHR)) {
      if (video_queue->videoCodecOperations & video_codec_operations) {
        return i;
      }
    }
  }
  return std::nullopt;
}

bool DeviceQueue::Initialize(const std::shared_ptr<DeviceQueue>& device_queue) {
  // queue family
  std::optional<uint32_t> graphics_queue_family_index = GraphicsQueueFamilyIndex(physical_device_);
  std::optional<uint32_t> video_queue_family_index = VideoQueueFamilyIndex(physical_device_);
  // make sure we get a valid index
  if (!graphics_queue_family_index || !video_queue_family_index) {
    return false;
  }
  // device
  VkDevice device = nullptr;
  {
    const float queue_priority = 0.0f;
    std::vector<VkDeviceQueueCreateInfo> queues = {
      VkDeviceQueueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = graphics_queue_family_index.value(),
        .queueCount = 1,
        .pQueuePriorities = &queue_priority
      },
      VkDeviceQueueCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = video_queue_family_index.value(),
        .queueCount = 1,
        .pQueuePriorities = &queue_priority
      },
    };
    // logical device
    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = (uint32_t)queues.size();
    device_info.pQueueCreateInfos = queues.data();
    // extensions
    std::vector<const char*> extension_names = {
      "VK_KHR_video_queue",
      "VK_KHR_video_decode_queue",
      "VK_EXT_video_decode_h264",
      "VK_KHR_synchronization2"
    };
    device_info.enabledExtensionCount = extension_names.size();
    device_info.ppEnabledExtensionNames = extension_names.data();
    vkCreateDevice(physical_device_->Handle(), &device_info, nullptr, &device);
  }
  // device
  device_ = std::make_shared<vk::Device>(device_queue);
  device_->Initialize(device);
  // graphics
  {
    VkQueue graphics_queue = nullptr;
    vkGetDeviceQueue(device, graphics_queue_family_index.value(), 0, &graphics_queue);
    graphics_queue_ = std::make_shared<vk::Queue>(device_queue, graphics_queue_family_index.value());
    graphics_queue_->Initialize(graphics_queue);
  }
  // video
  {
    VkQueue video_queue = nullptr;
    vkGetDeviceQueue(device, video_queue_family_index.value(), 0, &video_queue);
    video_queue_ = std::make_shared<vk::Queue>(device_queue, video_queue_family_index.value());
    video_queue_->Initialize(video_queue);
  }
  return true;
}

DeviceQueue::~DeviceQueue() {
}


}  // namespace vk
