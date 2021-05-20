
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}


#include <memory>
#include <vector>

namespace video {
class Demux;
}

namespace vk {

class CommandPool;
class VideoSessionMemories;
class VideoReferenceSlot;
class VideoBitstreamBuffer;
class VideoSessionParameters;

class VideoDecodeSession {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::VideoSessionMemories> memories_;
  std::vector<std::shared_ptr<vk::VideoReferenceSlot>> slots_;
  std::shared_ptr<vk::VideoBitstreamBuffer> bitstream_buffer_;
  std::shared_ptr<vk::VideoSessionParameters> parameters_;

  VkVideoSessionKHR video_session_;

 public:
  explicit VideoDecodeSession(
    const std::shared_ptr<vk::CommandPool>& command_pool);
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  bool Initialize(const std::unique_ptr<video::Demux>& demux);
  void Begin();

  const std::shared_ptr<vk::VideoBitstreamBuffer>& BitstreamBuffer() const { return bitstream_buffer_; }
};

}  // namespace vk
