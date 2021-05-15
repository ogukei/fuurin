
#include "vk/image_view.h"

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"

namespace vk {

std::shared_ptr<ImageView> ImageView::Create(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect_mask) {
  auto instance = std::make_shared<ImageView>(device, image, format, aspect_mask);
  instance->Initialize();
  return instance;
}

ImageView::ImageView(
    const std::shared_ptr<vk::Device>& device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspect_mask)
    : device_(device),
      image_(image),
      image_view_(nullptr),
      format_(format),
      aspect_mask_(aspect_mask) {
}

void ImageView::Initialize() {
  VkImageViewCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .image = image_,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = format_,
    .components = {},
    .subresourceRange = {
      .aspectMask = aspect_mask_,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
    },
  };
  VkImageView image_view = nullptr;
  vkCreateImageView(device_->Handle(), &create_info, nullptr, &image_view);
  image_view_ = image_view;
}

ImageView::~ImageView() {
  vkDestroyImageView(device_->Handle(), image_view_, nullptr);
}

}  // namespace vk
