
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

struct VideoSliceLayerBitstreamSegment {
  const uint8_t* data;
  size_t size;
};

}  // namespace vk
