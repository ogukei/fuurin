
#include <memory>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

namespace vk {

class VideoDecodeSession {
 private:
  
 public:
  explicit VideoDecodeSession();
  VideoDecodeSession(const VideoDecodeSession &) = delete;
  ~VideoDecodeSession();

  void Initialize();
};

} // namespace vk
