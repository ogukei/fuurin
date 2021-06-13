
#include "vk/device.h"

#include "vk/device_queue.h"

namespace vk {

Device::Device(const std::shared_ptr<vk::DeviceQueue>& device_queue)
    : device_(nullptr),
      device_queue_(device_queue) {
  physical_device_ = device_queue->PhysicalDevice();
}

void Device::Initialize(VkDevice device) {
  device_ = device;
}

Device::~Device() {
  vkDestroyDevice(device_, nullptr);
  device_ = nullptr;
}

}  // namespace vk
