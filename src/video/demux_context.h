
#pragma once

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
}

#include <memory>
#include <string>
#include <optional>

#include "video/demux.h"

namespace video {

class PacketReader;
class FrameParser;

class DemuxContext : public Demux {
 private:
  AVFormatContext* context_;
  std::unique_ptr<video::PacketReader> reader_;
  std::unique_ptr<video::FrameParser> parser_;

  AVStream* stream_;
  std::optional<video::BitstreamPacket> packet_;

 public:
  DemuxContext(const DemuxContext&) = delete;
  explicit DemuxContext(const std::string& filename);

  virtual ~DemuxContext();

  virtual uint32_t Width() const { return (uint32_t)stream_->codecpar->width;  }
  virtual uint32_t Height() const { return (uint32_t)stream_->codecpar->height; }

  virtual const std::optional<video::BitstreamPacket>& NextPacket();
};

}  // namespace video
