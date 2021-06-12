
#pragma once

#include <memory>
#include <string>

#include "nvidia_video_parser/nvidia_video_parser.h"

namespace video {
namespace nvidia_video_parser {

class H264Parser {
 private:
  VulkanVideoDecodeParser* parser_;
 public:
  H264Parser();
  ~H264Parser();
};

}  // namespace nvidia_video_parser
}  // namespace video
