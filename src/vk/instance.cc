
#include "vk/instance.h"

#include <iostream>

#include "vk/debug.h"

namespace vk {

std::shared_ptr<Instance> Instance::Create() {
  auto instance = std::make_shared<Instance>();
  instance->Initialize();
  return instance;
}

Instance::Instance() {}

void Instance::Initialize() {
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Fuurin";
  app_info.pEngineName = "Fuurin";
  app_info.apiVersion = VK_API_VERSION_1_2;

  const char* layers[] = {"VK_LAYER_KHRONOS_validation"};
  const char* extensions[] = {"VK_EXT_debug_utils"};
  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.ppEnabledLayerNames = layers;
  instance_info.enabledLayerCount = 1;
  instance_info.ppEnabledExtensionNames = extensions;
  instance_info.enabledExtensionCount = 1;

  VkInstance instance = nullptr;
  vkCreateInstance(&instance_info, nullptr, &instance);
  instance_ = instance;
  // debug
  debug_utils_messenger_ = vk::DebugUtilsMessenger::Create(instance);
}

Instance::~Instance() {
  // debug
  debug_utils_messenger_.reset(nullptr);
  // destroy
  vkDestroyInstance(instance_, nullptr);
  instance_ = nullptr;
}

}  // namespace vk
