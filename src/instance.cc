
#include "instance.h"

namespace vk {

std::shared_ptr<Instance> Instance::Create() {
  auto instance = std::make_shared<Instance>();
  instance->Initialize();
  return instance;
}

Instance::Instance() {

}

void Instance::Initialize() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Fuurin";
  app_info.pEngineName = "Fuurin";
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;

  VkInstance instance = nullptr;
  vkCreateInstance(&instance_info, nullptr, &instance);
  instance_ = instance;
}

Instance::~Instance() {
  vkDestroyInstance(instance_, nullptr);
  instance_ = nullptr;
}

} // namespace vk
