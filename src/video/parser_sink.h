
#pragma once

#include <memory>

namespace vk {
class H264PictureParameters;
class H264PictureInfo;
}  // namespace vk

namespace video {

struct VideoParserSink {
  virtual void OnParsePicture(const std::shared_ptr<vk::H264PictureInfo>& picture_info) = 0;
};

}  // namespace video
