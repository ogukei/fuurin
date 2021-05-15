
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;

class RenderPass {
 private:
  std::shared_ptr<vk::Device> device_;
  VkRenderPass render_pass_;

  void Initialize();

 public:
  static std::shared_ptr<RenderPass> Create(
    const std::shared_ptr<vk::Device>& device);

  explicit RenderPass(
    const std::shared_ptr<vk::Device>& device);
  RenderPass(const RenderPass&) = delete;
  ~RenderPass();

  VkRenderPass Handle() const { return render_pass_; }
};

}  // namespace vk
