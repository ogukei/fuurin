
#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
}

#include <memory>
#include <string>
#include <optional>

namespace video {

class FrameParser {
 private:
  AVCodecParserContext* codec_parser_context_;
  AVCodecContext* codec_context_;

  bool Initialize();
 public:
  static std::optional<std::unique_ptr<FrameParser>> Create(AVFormatContext* format_context);

  explicit FrameParser(AVFormatContext* format_context);

  void ParseWithPacket(AVPacket* packet);

  ~FrameParser();
};

}  // namespace video
