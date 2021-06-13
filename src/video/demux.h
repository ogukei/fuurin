
#pragma once

#include <memory>
#include <string>
#include <optional>

#include "video/bitstream_packet.h"

namespace video {

struct Demux {
  virtual ~Demux() {}

  virtual uint32_t Width() const = 0;
  virtual uint32_t Height() const = 0;

  virtual const std::optional<video::BitstreamPacket>& NextPacket() = 0;
};

std::unique_ptr<Demux> CreateDemux(const std::string& filename);

}  // namespace video
