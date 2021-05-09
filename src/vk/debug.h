
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Instance;

class DebugUtilsMessenger {
 private:
  VkInstance instance_;
  VkDebugUtilsMessengerEXT messenger_;

  void Initialize();
 public:
  static std::unique_ptr<DebugUtilsMessenger> Create(VkInstance instance);

  explicit DebugUtilsMessenger(VkInstance instance);
  DebugUtilsMessenger(const DebugUtilsMessenger&) = delete;
  ~DebugUtilsMessenger();
};

}  // namespace vk
