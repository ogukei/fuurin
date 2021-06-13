
#include "video/packet_reader.h"

#include <iostream>

namespace video {

PacketReader::PacketReader(
    AVFormatContext* context, int stream_index)
    : context_(context),
      stream_index_(stream_index) {
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

bool PacketReader::NextFrame() {
  if (packet_->data) {
    av_packet_unref(packet_);
  }
  if (packet_filtered_->data) {
    av_packet_unref(packet_filtered_);
  }
  int e = 0;
  while ((e = av_read_frame(context_, packet_)) >= 0) {
    // ignore mismatched streams
    if (packet_->stream_index == stream_index_) {
      break;
    } else {
      // mismatched
    }
    av_packet_unref(packet_);
  }
  if (e < 0) return false;
  av_bsf_send_packet(bsf_, packet_);
  av_bsf_receive_packet(bsf_, packet_filtered_);
  return true;
}

PacketReader::~PacketReader() {
  av_bsf_free(&bsf_);
  av_packet_free(&packet_);
  av_packet_free(&packet_filtered_);
}

}  // namespace video
