
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

class DemuxContext : public Demux {
 private:
  AVFormatContext* context_;
  std::unique_ptr<PacketReader> reader_;

  AVStream* stream_;
  std::optional<video::BitstreamSegment> segment_;

 public:
  DemuxContext(const DemuxContext&) = delete;
  explicit DemuxContext(const std::string& filename);

  virtual ~DemuxContext();

  virtual uint32_t Width() const { return (uint32_t)stream_->codecpar->width;  }
  virtual uint32_t Height() const { return (uint32_t)stream_->codecpar->height; }

  virtual const std::optional<video::BitstreamSegment>& NextSegment();
};

}  // namespace video
