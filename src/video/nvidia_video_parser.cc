
#include "video/nvidia_video_parser.h"

#include <iostream>

#include "video/bitstream_packet.h"
#include "video/nvidia_video_parser/h264_parser.h"

namespace video {

NvidiaVideoParser::NvidiaVideoParser() {
  parser_ = std::make_unique<nvidia_video_parser::H264Parser>();
  parser_->Initialize();
  parser_->RegisterSink(this);
}

void NvidiaVideoParser::Parse(const BitstreamPacket& bitstream_packet) {
  parser_->Parse(bitstream_packet);
}

const std::shared_ptr<vk::H264PictureParameters>& NvidiaVideoParser::PictureParameters() const {
  return parser_->PictureParameters();
}

bool NvidiaVideoParser::IsSequenceReady() const {
  return parser_->IsSequenceReady();
}

void NvidiaVideoParser::OnParsePicture(const std::shared_ptr<vk::H264PictureInfo>& picture_info) {
  if (callback_) {
    callback_(picture_info);
  }
}

NvidiaVideoParser::~NvidiaVideoParser() {
}

}  // namespace video
