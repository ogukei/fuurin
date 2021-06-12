
#pragma once

#include <memory>
#include <string>

namespace video {

namespace nvidia_video_parser {
class H264Parser;
}

class NvidiaVideoParser {
 private:
  std::unique_ptr<nvidia_video_parser::H264Parser> parser_;
 public:
  NvidiaVideoParser();
  ~NvidiaVideoParser();
};

}  // namespace video
