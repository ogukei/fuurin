
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>
#include <string>

namespace vk {

class Framebuffer;
class CommandPool;
class CommandBuffer;
class Image;
class ImageMemory;

class OffscreenRender {
 private:
  std::shared_ptr<vk::Framebuffer> framebuffer_;
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::CommandBuffer> command_buffer_;
  std::shared_ptr<vk::Image> image_;
  std::shared_ptr<vk::ImageMemory> image_memory_;

  std::optional<VkSubresourceLayout> layout_;

  void* mapped_data_;

  void Initialize();

 public:
  static std::shared_ptr<OffscreenRender> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::Framebuffer>& framebuffer);

  explicit OffscreenRender(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::Framebuffer>& framebuffer);

  OffscreenRender(const OffscreenRender&) = delete;
  ~OffscreenRender();

  void Execute();
  // saves as a PPM format file
  void Save(const std::string& filename);
};

}  // namespace vk
