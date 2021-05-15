
#include "vk/framebuffer.h"

#include <array>

#include "vk/device.h"
#include "vk/image.h"
#include "vk/image_memory.h"
#include "vk/image_view.h"
#include "vk/render_pass.h"

namespace vk {

std::shared_ptr<Framebuffer> Framebuffer::Create(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height) {
  auto frame_buffer = std::make_shared<Framebuffer>(device, width, height);
  frame_buffer->Initialize();
  return frame_buffer;
}

Framebuffer::Framebuffer(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height)
    : device_(device), width_(width), height_(height), framebuffer_(nullptr) {
}

void Framebuffer::Initialize() {
  // attachments
  // color
  color_image_ = Image::Create(
    device_,
    width_,
    height_,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
  color_image_view_ = ImageView::Create(
    device_,
    color_image_->Handle(),
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_IMAGE_ASPECT_COLOR_BIT);
  // depth
  depth_image_ = Image::Create(
    device_,
    width_,
    height_,
    VK_FORMAT_D32_SFLOAT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  depth_image_view_ = ImageView::Create(
    device_,
    depth_image_->Handle(),
    VK_FORMAT_D32_SFLOAT,
    VK_IMAGE_ASPECT_DEPTH_BIT);
  // render pass
  render_pass_ = RenderPass::Create(device_);
  // attachments
  std::array<VkImageView, 2> attachments = {
    color_image_view_->Handle(),
    depth_image_view_->Handle()
  };
  // create
  VkFramebufferCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .renderPass = render_pass_->Handle(),
    .attachmentCount = (uint32_t)attachments.size(),
    .pAttachments = attachments.data(),
    .width = width_,
    .height = height_,
    .layers = 1,
  };
  VkFramebuffer framebuffer = nullptr;
  vkCreateFramebuffer(device_->Handle(), &create_info, nullptr, &framebuffer);
  framebuffer_ = framebuffer;
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(device_->Handle(), framebuffer_, nullptr);
  framebuffer_ = nullptr;
}

}  // namespace vk
