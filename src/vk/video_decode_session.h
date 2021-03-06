
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}


#include <memory>
#include <vector>
#include <optional>
#include <string>

namespace video {
class Demux;
}

namespace vk {

class CommandPool;
class VideoSessionMemories;
class VideoReferenceSlot;
class VideoBitstreamBuffer;
class VideoSessionParameters;
class VideoDecodeSurface;
class VideoReferenceState;
class VideoQueryPool;
class H264PictureParameters;
class H264PictureInfo;

class VideoDecodeSession {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::VideoSessionMemories> memories_;
  std::vector<std::shared_ptr<vk::VideoDecodeSurface>> decode_surfaces_;
  std::shared_ptr<vk::VideoReferenceState> reference_state_;
  std::shared_ptr<vk::VideoBitstreamBuffer> bitstream_buffer_;
  std::shared_ptr<vk::VideoSessionParameters> parameters_;
  std::shared_ptr<vk::H264PictureParameters> picture_parameters_;
  std::shared_ptr<vk::VideoQueryPool> video_query_pool_;

  VkVideoSessionKHR video_session_;
  VkExtent2D extent_;

  bool Initialize(const std::unique_ptr<video::Demux>& demux);

 public:
  static std::optional<std::unique_ptr<VideoDecodeSession>> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::unique_ptr<video::Demux>& demux,
    const std::shared_ptr<vk::VideoBitstreamBuffer>& bitstream_buffer,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters);

  explicit VideoDecodeSession(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::VideoBitstreamBuffer>& bitstream_buffer,
    const std::shared_ptr<vk::H264PictureParameters>& picture_parameters);
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  void Begin(const std::shared_ptr<vk::H264PictureInfo>& picture_info);

  const std::shared_ptr<vk::VideoBitstreamBuffer>& BitstreamBuffer() const { return bitstream_buffer_; }

  void DumpPicture(const std::string& filename);
};

}  // namespace vk
