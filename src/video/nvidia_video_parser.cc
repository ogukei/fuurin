
#include "video/nvidia_video_parser.h"

#include <iostream>

#include "video/nvidia_video_parser/h264_parser.h"

namespace video {

NvidiaVideoParser::NvidiaVideoParser() {
  parser_ = std::make_unique<nvidia_video_parser::H264Parser>();
}

NvidiaVideoParser::~NvidiaVideoParser() {
}

}  // namespace video
