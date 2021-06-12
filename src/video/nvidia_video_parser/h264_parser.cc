
#include "video/nvidia_video_parser/h264_parser.h"

#include <iostream>


namespace video {
namespace nvidia_video_parser {

H264Parser::H264Parser() : parser_(nullptr) {
  bool ok = CreateVulkanVideoDecodeParser(&parser_, VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT);
  assert(ok);
}

H264Parser::~H264Parser() {
  if (parser_ != nullptr) {
    parser_->Deinitialize();
    parser_->Release();
    parser_ = nullptr;
  }
}

}  // namespace nvidia_video_parser
}  // namespace video
