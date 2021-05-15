
#include "vk/shader_module.h"

#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>

#include "vk/device.h"

namespace vk {

std::optional<std::shared_ptr<ShaderModule>> ShaderModule::Create(
    const std::shared_ptr<vk::Device> &device,
    const std::string& filename) {
  auto shader_module = std::make_shared<ShaderModule>(device, filename);
  return (shader_module->Initialize()) ? std::optional {shader_module} : std::nullopt;
}

ShaderModule::ShaderModule(
    const std::shared_ptr<vk::Device> &device,
    const std::string& filename)
    : filename_(filename),
      shader_module_(nullptr),
      device_(device) {
}

bool ShaderModule::Initialize() {
  std::ifstream input(filename_, std::ios::binary);
  if (!input.good()) return false;
  std::istreambuf_iterator<char> it(input), eof;
  std::vector<uint8_t> buffer(it, eof);
  // shader
  VkShaderModuleCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .codeSize = (size_t)buffer.size(),
    .pCode = (const uint32_t*)buffer.data()
  };
  VkShaderModule shader_module = nullptr;
  vkCreateShaderModule(device_->Handle(), &create_info, nullptr, &shader_module);
  if (shader_module == nullptr) return false;
  shader_module_ = shader_module;
  return true;
}

ShaderModule::~ShaderModule() {
  vkDestroyShaderModule(device_->Handle(), shader_module_, nullptr);
  shader_module_ = nullptr;
}

}  // namespace vk
