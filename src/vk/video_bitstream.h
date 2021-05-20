
#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <deque>
#include <optional>

namespace video {
class BitstreamSegment;
}

namespace vk {

class CommandPool;
class BufferMemory;

struct VideoBitstreamSegmentReference {
  VkDeviceSize region_size;
  VkDeviceSize size;
  VkDeviceSize offset;
};

class VideoBitstreamBuffer {
 private:
  std::shared_ptr<vk::CommandPool> command_pool_;
  std::shared_ptr<vk::BufferMemory> buffer_memory_;

  std::deque<vk::VideoBitstreamSegmentReference> segment_references_;
  VkDeviceSize offset_;

  void Initialize();
  void Write(
    void *data,
    const vk::VideoBitstreamSegmentReference& segment_reference);

 public:
  static std::shared_ptr<VideoBitstreamBuffer> Create(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  explicit VideoBitstreamBuffer(
    const std::shared_ptr<vk::CommandPool>& command_pool);

  VideoBitstreamBuffer(const VideoBitstreamBuffer&) = delete;
  ~VideoBitstreamBuffer();

  void AppendSegment(video::BitstreamSegment segment);
  std::optional<vk::VideoBitstreamSegmentReference> PrependSegmentReference();

  VkBuffer Buffer() const;
};

}  // namespace vk
