
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Instance {
 private:
  VkInstance instance_;
 public:
  static std::shared_ptr<Instance> Create();

  Instance();
  Instance(const Instance &) = delete;
  ~Instance();

  void Initialize();
  VkInstance Handle() const { return instance_; }
};

}  // namespace vk
