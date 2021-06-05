
#pragma once

extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}


#include <memory>
#include <vector>
#include <optional>

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

class VideoDecodeSession {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::VideoSessionMemories> memories_;
  std::vector<std::shared_ptr<vk::VideoDecodeSurface>> decode_surfaces_;
  std::vector<std::shared_ptr<vk::VideoReferenceSlot>> reference_slots_;
  std::shared_ptr<vk::VideoBitstreamBuffer> bitstream_buffer_;
  std::shared_ptr<vk::VideoSessionParameters> parameters_;

  VkVideoSessionKHR video_session_;
  VkExtent2D extent_;

  bool Initialize(const std::unique_ptr<video::Demux>& demux);

 public:
  static std::optional<std::unique_ptr<VideoDecodeSession>> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::unique_ptr<video::Demux>& demux);

  explicit VideoDecodeSession(
    const std::shared_ptr<vk::CommandPool>& command_pool);
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  void Begin();

  const std::shared_ptr<vk::VideoBitstreamBuffer>& BitstreamBuffer() const { return bitstream_buffer_; }
};

}  // namespace vk
