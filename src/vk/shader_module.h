
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <string>
#include <optional>

namespace vk {

class Device;

class ShaderModule {
 private:
  std::shared_ptr<vk::Device> device_;
  std::string filename_;
  VkShaderModule shader_module_;

  bool Initialize();
 public:
  static std::optional<std::shared_ptr<ShaderModule>> Create(
    const std::shared_ptr<vk::Device> &device,
    const std::string& filename);

  explicit ShaderModule(
    const std::shared_ptr<vk::Device> &device,
    const std::string& filename);
  ShaderModule(const ShaderModule&) = delete;
  ~ShaderModule();

  VkShaderModule Handle() const { return shader_module_; }
};

}  // namespace vk
