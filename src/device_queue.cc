
#include "device_queue.h"

#include <vector>

#include "physical_device.h"
#include "queue.h"
#include "device.h"

namespace vk {

std::optional<std::shared_ptr<DeviceQueue>> DeviceQueue::Create(const std::shared_ptr<vk::PhysicalDevice>& physical_device) {
  auto device_queue = std::make_shared<DeviceQueue>(physical_device);
  if (device_queue->Initialize(device_queue)) {
    return device_queue;
  } else {
    return std::nullopt;
  }
}

DeviceQueue::DeviceQueue(const std::shared_ptr<vk::PhysicalDevice>& physical_device) : physical_device_(physical_device) {

}

bool DeviceQueue::Initialize(const std::shared_ptr<DeviceQueue>& device_queue) {
  // queue family
  std::optional<uint32_t> queue_family_index;
  {
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_->Handle(), &family_count, nullptr);
    std::vector<VkQueueFamilyProperties> families;
    families.resize(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_->Handle(), &family_count, families.data());
    {
      for (uint32_t i = 0; i < family_count; i++) {
        const VkQueueFamilyProperties& properties = families.at(i);
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          queue_family_index = i;
          break;
        }
      }
    }
  }
  // make sure we get a valid index
  if (!queue_family_index) {
    return false;
  }
  queue_family_index_ = queue_family_index;
  // device
  VkDevice device = nullptr;
  {
    const float queue_priority = 0.0f;
    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueCount = 1;
    queue_info.queueFamilyIndex = queue_family_index.value();
    queue_info.pQueuePriorities = &queue_priority;
    // logical device
    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    // extensions
    std::vector<const char*> extension_names = {
      "VK_EXT_video_decode_h264"
    };
    device_info.enabledExtensionCount = 1;
    device_info.ppEnabledExtensionNames = extension_names.data();
    vkCreateDevice(physical_device_->Handle(), &device_info, nullptr, &device);
  }
  // queue handle
  VkQueue queue = nullptr;
  vkGetDeviceQueue(device, queue_family_index.value(), 0, &queue);
  // device
  device_ = std::make_shared<vk::Device>(device_queue);
  device_->Initialize(device);
  // queue
  queue_ = std::make_shared<vk::Queue>(device_queue);
  queue_->Initialize(queue);
  return true;
}

DeviceQueue::~DeviceQueue() {

}


} // namespace vk
