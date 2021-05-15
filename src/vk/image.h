
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

class ColorImage {
 private:
  uint32_t width_;
  uint32_t height_;
  void Initialize();
 public:
  static std::shared_ptr<ColorImage> Create(uint32_t width, uint32_t height);

  explicit ColorImage(uint32_t width, uint32_t height);
  ColorImage(const ColorImage&) = delete;
  ~ColorImage();
};

}  // namespace vk
