
#include <memory>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

namespace vk {

class DeviceQueue;

class VideoDecodeSession {
 private:
  std::shared_ptr<vk::DeviceQueue> device_queue_;
 public:
  explicit VideoDecodeSession(const std::shared_ptr<vk::DeviceQueue>& device_queue);
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  void Initialize();
};

} // namespace vk
