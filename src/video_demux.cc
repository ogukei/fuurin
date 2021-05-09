
#include "video_demux.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavcodec/avcodec.h>
}

#include <iostream>

namespace video {

namespace {

class PacketReader {
 private:
  AVFormatContext* context_;
  AVPacket* packet_;
  AVPacket* packet_filtered_;
  int stream_index_;
  AVBSFContext* bsf_;

 public:
  PacketReader(AVFormatContext* context, int stream_index) 
    : context_(context), stream_index_(stream_index) {
    // packet
    packet_ = av_packet_alloc();
    av_init_packet(packet_);
    packet_filtered_ = av_packet_alloc();
    av_init_packet(packet_filtered_);
    // bsf
    // assume H264
    const AVBitStreamFilter* bsf = av_bsf_get_by_name("h264_mp4toannexb");
    av_bsf_alloc(bsf, &bsf_);
    // ? @see https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/NvCodecUtils/FFmpegDemuxer.h#L233
    bsf_->par_in = context->streams[stream_index]->codecpar;
    av_bsf_init(bsf_);
  }

  bool Next() {
    if (packet_->data) {
      av_packet_unref(packet_);
    }
    if (packet_filtered_->data) {
      av_packet_unref(packet_filtered_);
    }
    int e = 0;
    while ((e = av_read_frame(context_, packet_)) >= 0) {
      // ignore mismatched streams
      if (packet_->stream_index == stream_index_) break;
      av_packet_unref(packet_);
    }
    if (e < 0) return false;
    av_bsf_send_packet(bsf_, packet_);
    av_bsf_receive_packet(bsf_, packet_filtered_);
    return true;
  }

  uint8_t* Data() const { return packet_filtered_->data; }
  int Size() const { return packet_filtered_->size; }

  ~PacketReader() {
    av_bsf_free(&bsf_);
    av_packet_free(&packet_);
    av_packet_free(&packet_filtered_);
  }
};

class DemuxContext : public Demux {
 private:
  AVFormatContext* context_;
  PacketReader* reader_;

  AVStream* stream_;
 public:
  DemuxContext(const DemuxContext&) = delete;
  DemuxContext() : reader_(nullptr) {
    av_register_all();
    avformat_network_init();

    AVFormatContext* context = nullptr;
    if (avformat_open_input(&context, "/home/user/Downloads/BigBuckBunny.mp4", nullptr, nullptr) != 0) {
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
    reader_ = new PacketReader(context, stream_index);
  }
  virtual ~DemuxContext() {
    stream_ = nullptr;
    delete reader_;
    reader_ = nullptr;
    avformat_close_input(&context_);
  }
  virtual uint32_t Width() const { return (uint32_t)stream_->codecpar->width;  }
  virtual uint32_t Height() const { return (uint32_t)stream_->codecpar->height; }
};

} // namespace

std::unique_ptr<Demux> CreateDemux() {
  return std::make_unique<DemuxContext>();
}

} // namespace video
