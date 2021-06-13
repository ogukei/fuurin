
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <vector>

namespace vk {

class Device;
class VideoSessionMemory;

class VideoSessionMemories {
 private:
  std::shared_ptr<vk::Device> device_;
  VkVideoSessionKHR video_session_;
  std::vector<std::shared_ptr<vk::VideoSessionMemory>> memories_;

  void Initialize();
 public:
  static std::shared_ptr<VideoSessionMemories> Create(
    const std::shared_ptr<vk::Device>& device,
    VkVideoSessionKHR video_session);

  explicit VideoSessionMemories(
    const std::shared_ptr<vk::Device>& device,
    VkVideoSessionKHR video_session);

  VideoSessionMemories(const VideoSessionMemories&) = delete;
  ~VideoSessionMemories();

  size_t Count() const { return memories_.size(); }
};

}  // namespace vk
