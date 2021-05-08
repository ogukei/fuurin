
#include "device.h"

namespace vk {

Device::Device(const std::shared_ptr<vk::DeviceQueue>& device_queue) : device_(nullptr), device_queue_(device_queue) {

}

void Device::Initialize(VkDevice device) {
  device_ = device;
}

Device::~Device() {
  vkDestroyDevice(device_, nullptr);
  device_ = nullptr;
}

} // namespace vk
