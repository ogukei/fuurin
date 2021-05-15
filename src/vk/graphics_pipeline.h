
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class Framebuffer;

class GraphicsPipeline {
 private:
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::Framebuffer> framebuffer_;
  VkPipeline pipeline_;
  VkDescriptorSetLayout descriptor_set_layout_;
  VkPipelineLayout pipeline_layout_;
  VkPipelineCache pipeline_cache_;

  void Initialize();

 public:
  static std::shared_ptr<GraphicsPipeline> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Framebuffer>& framebuffer);

  explicit GraphicsPipeline(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Framebuffer>& framebuffer);
  GraphicsPipeline(const GraphicsPipeline&) = delete;
  ~GraphicsPipeline();
};

}  // namespace vk
