
#include "vk/image.h"

namespace vk {

std::shared_ptr<ColorImage> ColorImage::Create(uint32_t width, uint32_t height) {
  auto instance = std::make_shared<ColorImage>(width, height);
  instance->Initialize();
  return instance;
}

ColorImage::ColorImage(uint32_t width, uint32_t height)
    : width_(width),
      height_(height) {
}

void ColorImage::Initialize() {
  VkImageCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R8G8B8A8_UNORM,
    .extent = {
      .width = width_,
      .height = height_,
      .depth = 1,
    },
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };
}

ColorImage::~ColorImage() {
}

}  // namespace vk
