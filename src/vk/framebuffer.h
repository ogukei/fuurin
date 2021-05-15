
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class Device;
class Image;
class ImageView;
class RenderPass;

class Framebuffer {
 private:
  std::shared_ptr<vk::Device> device_;
  uint32_t width_;
  uint32_t height_;
  // attachments
  std::shared_ptr<vk::Image> color_image_;
  std::shared_ptr<vk::ImageView> color_image_view_;
  std::shared_ptr<vk::Image> depth_image_;
  std::shared_ptr<vk::ImageView> depth_image_view_;
  std::shared_ptr<vk::RenderPass> render_pass_;
  // handle
  VkFramebuffer framebuffer_;

  void Initialize();

 public:
  static std::shared_ptr<Framebuffer> Create(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height);

  explicit Framebuffer(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height);
  Framebuffer(const Framebuffer&) = delete;
  ~Framebuffer();

  const std::shared_ptr<vk::RenderPass>& RenderPass() const { return render_pass_; }
};

}  // namespace vk
