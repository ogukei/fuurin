
#pragma once

#include <memory>

namespace video {

struct BitstreamSegment {
  uint8_t* data;
  size_t size;
  // metadata
  int64_t presentation_timestamp;
  int64_t decompression_timestamp;
};

}  // namespace video
