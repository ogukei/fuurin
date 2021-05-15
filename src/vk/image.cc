
#include "vk/image.h"

#include "vk/device.h"
#include "vk/image_memory.h"

namespace vk {

std::shared_ptr<Image> Image::Create(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height,
    VkImageUsageFlags usage,
    VkFormat format) {
  auto instance = std::make_shared<Image>(device, width, height, usage, format);
  instance->Initialize();
  return instance;
}

Image::Image(
    const std::shared_ptr<vk::Device>& device,
    uint32_t width,
    uint32_t height,
    VkImageUsageFlags usage,
    VkFormat format)
    : width_(width),
      height_(height),
      usage_(usage),
      format_(format),
      image_(nullptr),
      device_(device) {
}

void Image::Initialize() {
  VkImageCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = format_,
    .extent = {
      .width = width_,
      .height = height_,
      .depth = 1,
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = usage_,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
  VkImage image = nullptr;
  vkCreateImage(device_->Handle(), &create_info, nullptr, &image);
  image_ = image;
  // image memory
  image_memory_ = ImageMemory::Create(image, device_);
}

Image::~Image() {
  vkDestroyImage(device_->Handle(), image_, nullptr);
}

}  // namespace vk
