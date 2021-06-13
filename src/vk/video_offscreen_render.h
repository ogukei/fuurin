
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>
#include <string>

namespace vk {

class CommandPool;
class CommandBuffer;
class Image;
class ImageMemory;
class VideoSessionFrame;

class VideoOffscreenRender {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::VideoSessionFrame> frame_;
  std::shared_ptr<vk::CommandBuffer> command_buffer_;
  std::shared_ptr<vk::Image> image_;
  std::shared_ptr<vk::ImageMemory> image_memory_;

  std::optional<VkSubresourceLayout> plane0_layout_;
  std::optional<VkSubresourceLayout> plane1_layout_;
  void* mapped_data_;

  void Initialize();

 public:
  static std::shared_ptr<VideoOffscreenRender> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::VideoSessionFrame>& frame);

  explicit VideoOffscreenRender(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::VideoSessionFrame>& frame);

  VideoOffscreenRender(const VideoOffscreenRender&) = delete;
  ~VideoOffscreenRender();

  void Execute();
  // saves as a PPM format file
  void Save(const std::string& filename);
};

}  // namespace vk
