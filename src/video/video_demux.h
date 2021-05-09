
#pragma once

#include <memory>

namespace video {

struct Demux {
  virtual ~Demux() {}

  virtual uint32_t Width() const = 0;
  virtual uint32_t Height() const = 0;
};

std::unique_ptr<Demux> CreateDemux();

}  // namespace video
