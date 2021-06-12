
#pragma once

#include <memory>
#include <string>

namespace vk {
class H264PictureParameters;
}  // namespace vk

namespace video {

class BitstreamSegment;

namespace nvidia_video_parser {
class H264Parser;
}

class NvidiaVideoParser {
 private:
  std::unique_ptr<nvidia_video_parser::H264Parser> parser_;
 public:
  NvidiaVideoParser();

  void Parse(const BitstreamSegment& bitstream_segment);

  const std::shared_ptr<vk::H264PictureParameters>& PictureParameters() const;
  bool IsSequenceReady() const;

  ~NvidiaVideoParser();
};

}  // namespace video
