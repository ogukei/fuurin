
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

class DeviceQueue;
class VideoSessionMemories;

class VideoDecodeSession {
 private:
  std::shared_ptr<vk::DeviceQueue> device_queue_;
  std::unique_ptr<video::Demux> video_demux_;
  std::shared_ptr<vk::VideoSessionMemories> memories_;

  VkVideoSessionKHR video_session_;
 public:
  explicit VideoDecodeSession(const std::shared_ptr<vk::DeviceQueue>& device_queue);
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  bool Initialize();
};

}  // namespace vk
