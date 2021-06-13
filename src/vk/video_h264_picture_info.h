

#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>
#include <vector>
#include <utility>

#include "vk/video_bitstream_segment.h"
#include "vk/video_h264_dpb_entry.h"

namespace vk {

class H264PictureInfo {
 private:
  std::optional<StdVideoDecodeH264PictureInfo> info_;
  uint32_t slices_count_;
  const uint32_t* slices_data_offsets_;

  uint32_t bitstream_data_size_;
  const uint8_t* bitstream_data_;

  std::vector<vk::H264DecodedPictureBufferEntry> dpb_entries_;
  uint32_t frame_index_;

 public:
  explicit H264PictureInfo(
      const StdVideoDecodeH264PictureInfo& info,
      uint32_t bitstream_data_size,
      const uint8_t* bitstream_data,
      uint32_t slices_count,
      const uint32_t* slices_data_offsets,
      std::vector<vk::H264DecodedPictureBufferEntry>&& dpb_entries,
      uint32_t frame_index)
      : info_(info),
        bitstream_data_size_(bitstream_data_size),
        bitstream_data_(bitstream_data),
        slices_count_(slices_count),
        slices_data_offsets_(slices_data_offsets),
        dpb_entries_(std::forward<std::vector<vk::H264DecodedPictureBufferEntry>>(dpb_entries)),
        frame_index_(frame_index) {
  }

  H264PictureInfo(const H264PictureInfo&) = delete;
  ~H264PictureInfo() {}

  const StdVideoDecodeH264PictureInfo* StdPictureInfo() const { return &info_.value(); }
  uint32_t SlicesCount() const { return slices_count_; }
  const uint32_t* SlicesDataOffsets() const { return slices_data_offsets_; }

  const std::vector<vk::H264DecodedPictureBufferEntry>& DecodedPictureBufferEntries() const { return dpb_entries_; }

  uint32_t FrameIndex() const { return frame_index_; }

  vk::VideoSliceLayerBitstreamSegment BitstreamSegment() const {
    vk::VideoSliceLayerBitstreamSegment segment = {
      .data = bitstream_data_,
      .size = bitstream_data_size_,
    };
    return segment;
  }
};

}  // namespace vk
