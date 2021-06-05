
#include "video/frame_parser.h"

#include <iostream>
#include <utility>

namespace video {

std::optional<std::unique_ptr<FrameParser>> FrameParser::Create(
    AVFormatContext* format_context) {
  auto parser = std::make_unique<FrameParser>(format_context);
  bool ok = parser->Initialize();
  return (ok) ? std::optional {std::move(parser)} : std::nullopt;
}

FrameParser::FrameParser(AVFormatContext* format_context) {
}

bool FrameParser::Initialize() {
  const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  codec_parser_context_ = av_parser_init(codec->id);
  codec_context_ = avcodec_alloc_context3(codec);
  int ok;
  ok = avcodec_open2(codec_context_, codec, nullptr);
  if (ok < 0) return false;
  return true;
}

void FrameParser::ParseWithPacket(AVPacket* packet) {
  uint8_t* buffer = nullptr;
  int size = 0;

  int ok;
  ok = av_parser_parse2(codec_parser_context_, codec_context_,
    &buffer, &size,  // outputs
    packet->data, packet->size,  // inputs
    packet->pts, packet->dts, packet->pos);
  if (ok < 0) return;

  int picture_type = codec_parser_context_->pict_type;
  int key_frame = codec_parser_context_->key_frame;
  int repeat_picture = codec_parser_context_->repeat_pict;
}

FrameParser::~FrameParser() {
  av_parser_close(codec_parser_context_);
  codec_parser_context_ = nullptr;
  avcodec_free_context(&codec_context_);
}

}  // namespace video
