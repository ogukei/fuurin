
#include "video/nvidia_video_parser.h"

#include <iostream>

#include "video/bitstream_segment.h"
#include "video/nvidia_video_parser/h264_parser.h"

namespace video {

NvidiaVideoParser::NvidiaVideoParser() {
  parser_ = std::make_unique<nvidia_video_parser::H264Parser>();
  parser_->Initialize();
}

void NvidiaVideoParser::Parse(const BitstreamSegment& bitstream_segment) {
  parser_->Parse(bitstream_segment);
}

const std::shared_ptr<vk::H264PictureParameters>& NvidiaVideoParser::PictureParameters() const {
  return parser_->PictureParameters();
}

std::shared_ptr<vk::H264PictureInfo> NvidiaVideoParser::CurrentPictureInfo() const {
  return parser_->CurrentPictureInfo();
}

bool NvidiaVideoParser::IsSequenceReady() const {
  return parser_->IsSequenceReady();
}

NvidiaVideoParser::~NvidiaVideoParser() {
}

}  // namespace video
