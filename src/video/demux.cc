
#include "video/demux.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
}

#include "video/demux_context.h"

namespace video {

std::unique_ptr<Demux> CreateDemux(const std::string& filename) {
  return std::make_unique<DemuxContext>(filename);
}

}  // namespace video
