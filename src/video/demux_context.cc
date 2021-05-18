
#include "video/demux_context.h"

#include <iostream>

#include "video/packet_reader.h"

namespace video {

DemuxContext::DemuxContext(const std::string& filename)
    : reader_(nullptr) {
  av_register_all();
  avformat_network_init();

  AVFormatContext* context = nullptr;
  if (avformat_open_input(&context, filename.c_str(), nullptr, nullptr) != 0) {
    return;
  }
  if (avformat_find_stream_info(context, nullptr) != 0) {
    return;
  }
  int stream_index = av_find_best_stream(context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (stream_index < 0) {
    return;
  }
  auto* stream = context->streams[stream_index];
  context_ = context;
  // @see https://ffmpeg.org/doxygen/2.8/structAVStream.html
  stream_ = context->streams[stream_index];
  reader_ = std::make_unique<PacketReader>(context, stream_index);
}

const std::optional<video::BitstreamSegment>& DemuxContext::NextSegment() {
  if (reader_->NextFrame()) {
    segment_ = BitstreamSegment {
      .data = reader_->Data(),
      .size = static_cast<size_t>(reader_->Size()),
      .presentation_timestamp = reader_->Packet()->pts,
      .decompression_timestamp = reader_->Packet()->dts
    };
  } else {
    segment_ = std::nullopt;
  }
  return segment_;
}

DemuxContext::~DemuxContext() {
  stream_ = nullptr;
  reader_ = nullptr;
  avformat_close_input(&context_);
}

}  // namespace video
