
#pragma once

#include <memory>
#include <string>
#include <functional>

#include "video/parser_sink.h"

namespace vk {
class H264PictureParameters;
class H264PictureInfo;
}  // namespace vk

namespace video {

class BitstreamSegment;

namespace nvidia_video_parser {
class H264Parser;
}

class NvidiaVideoParser : public VideoParserSink {
 private:
  std::unique_ptr<nvidia_video_parser::H264Parser> parser_;
  std::function<void(const std::shared_ptr<vk::H264PictureInfo>&)> callback_;

 public:
  NvidiaVideoParser();

  void Parse(const BitstreamSegment& bitstream_segment);

  const std::shared_ptr<vk::H264PictureParameters>& PictureParameters() const;
  bool IsSequenceReady() const;

  void RegisterCallback(const std::function<void(const std::shared_ptr<vk::H264PictureInfo>&)>& function) {
    callback_ = function;
  }

  // VideoParserSink
  virtual void OnParsePicture(const std::shared_ptr<vk::H264PictureInfo>& picture_info);

  ~NvidiaVideoParser();
};

}  // namespace video
