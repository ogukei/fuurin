
#include "video_demux.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
}

namespace video {

namespace {

class DemuxImpl : public Demux {
 public:
  DemuxImpl() {
    av_register_all();
    avformat_network_init();

    AVFormatContext *context = nullptr;
    avformat_open_input(&context, "/home/user/Downloads/BigBuckBunny.mp4", nullptr, nullptr);
  }
};

} // namespace

std::unique_ptr<Demux> CreateDemux() {
  return std::make_unique<DemuxImpl>();
}

} // namespace video
