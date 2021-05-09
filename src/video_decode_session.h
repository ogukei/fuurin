
#include <memory>
#include <vector>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

namespace video {
  class Demux;
}

namespace vk {

class DeviceQueue;

class VideoDecodeSession {
 private:
  std::shared_ptr<vk::DeviceQueue> device_queue_;
  std::unique_ptr<video::Demux> video_demux_;
  VkVideoSessionKHR video_session_;
 public:
  explicit VideoDecodeSession(const std::shared_ptr<vk::DeviceQueue>& device_queue);
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  bool Initialize();

  void Setup();
};

} // namespace vk
