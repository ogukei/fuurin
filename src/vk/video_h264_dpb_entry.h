

#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>

namespace vk {

struct H264DecodedPictureBufferEntry {
  int32_t frame_index;
  int32_t reference_flags;
  int32_t is_long_term;
  int32_t is_non_existing;
  int32_t top_field_order_count;
  int32_t bottom_field_order_count;
  bool is_field_reference;
  bool is_top_field_flag;
  bool is_bottom_field_flag;
};

}  // namespace vk
