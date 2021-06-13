
#include "vk/debug.h"

#include <iostream>

namespace vk {

static VkBool32 debugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void *) {
  const char *message = data->pMessage;
  std::cout << message << std::endl;
  return VK_FALSE;
}

static PFN_vkCreateDebugUtilsMessengerEXT vk_vkCreateDebugUtilsMessengerEXT(VkInstance instance) {
  return (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
    instance, "vkCreateDebugUtilsMessengerEXT");
}

static PFN_vkDestroyDebugUtilsMessengerEXT vk_vkDestroyDebugUtilsMessengerEXT(VkInstance instance) {
  return (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
    instance, "vkDestroyDebugUtilsMessengerEXT");
}

std::unique_ptr<DebugUtilsMessenger> DebugUtilsMessenger::Create(VkInstance instance) {
  auto debug = std::make_unique<DebugUtilsMessenger>(instance);
  debug->Initialize();
  return debug;
}

DebugUtilsMessenger::DebugUtilsMessenger(VkInstance instance)
    : instance_(instance),
      messenger_(nullptr) {
}

void DebugUtilsMessenger::Initialize() {
  VkDebugUtilsMessengerCreateInfoEXT create_info = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = nullptr,
    .flags = 0,
    .messageSeverity = 0
    //  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = 0
      | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debugUtilsMessengerCallback,
    .pUserData = nullptr
  };
  VkDebugUtilsMessengerEXT messenger = nullptr;
  auto fn = vk_vkCreateDebugUtilsMessengerEXT(instance_);
  if (fn == nullptr) {
    return;
  }
  fn(instance_, &create_info, nullptr, &messenger);
  messenger_ = messenger;
}

DebugUtilsMessenger::~DebugUtilsMessenger() {
  auto fn = vk_vkDestroyDebugUtilsMessengerEXT(instance_);
  if (fn) {
    fn(instance_, messenger_, nullptr);
    messenger_ = nullptr;
  }
}

}  // namespace vk
