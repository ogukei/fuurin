
#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
}

#include <memory>
#include <string>

namespace video {

class PacketReader {
 private:
  AVFormatContext* context_;
  AVPacket* packet_;
  AVPacket* packet_filtered_;
  int stream_index_;
  AVBSFContext* bsf_;

 public:
  explicit PacketReader(AVFormatContext* context, int stream_index);

  bool NextFrame();

  uint8_t* Data() const { return packet_filtered_->data; }
  int Size() const { return packet_filtered_->size; }
  AVPacket* Packet() const { return packet_filtered_; }

  ~PacketReader();
};

}  // namespace video
