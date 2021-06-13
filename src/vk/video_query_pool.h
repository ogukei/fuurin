
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class Device;
class Queue;
class VideoProfile;

class VideoQueryPool {
 private:
  std::shared_ptr<vk::Device> device_;
  std::shared_ptr<vk::VideoProfile> video_profile_;
  uint32_t query_count_;
  VkQueryPool query_pool_;

  bool Initialize();

 public:
  static std::optional<std::shared_ptr<VideoQueryPool>> Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t query_count);

  explicit VideoQueryPool(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t query_count);

  VideoQueryPool(const VideoQueryPool &) = delete;
  ~VideoQueryPool();

  VkQueryPool Handle() const { return query_pool_; }

  const std::shared_ptr<vk::Device>& Device() const { return device_; }
  uint32_t QueryCount() const { return query_count_; }

  void DumpResult();
};

}  // namespace vk
