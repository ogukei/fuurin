
#include "physical_device.h"

#include <vector>

#include "instance.h"

namespace vk {

std::shared_ptr<PhysicalDevice> PhysicalDevice::Create(
    std::shared_ptr<vk::Instance>& instance
) {
  auto physical_device = std::make_shared<PhysicalDevice>(instance);
  physical_device->Initialize();
  return physical_device;
}

PhysicalDevice::PhysicalDevice(std::shared_ptr<vk::Instance>& instance) : instance_(instance) {

}

void PhysicalDevice::Initialize() {
  VkPhysicalDevice physical_device = nullptr;
  {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_->Handle(), &device_count, nullptr);
    std::vector<VkPhysicalDevice> devices;
    devices.resize(device_count);
    vkEnumeratePhysicalDevices(instance_->Handle(), &device_count, devices.data());
    physical_device = devices[0];
  }
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physical_device, &properties);
  physical_device_ = physical_device;
}

PhysicalDevice::~PhysicalDevice() {
  
}

} // namespace vk
