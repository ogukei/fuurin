
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class DebugUtilsMessenger;

class Instance {
 private:
  VkInstance instance_;
  std::unique_ptr<DebugUtilsMessenger> debug_utils_messenger_;

  void Initialize();
 public:
  static std::shared_ptr<Instance> Create();

  Instance();
  Instance(const Instance&) = delete;
  ~Instance();

  VkInstance Handle() const { return instance_; }
};

}  // namespace vk
