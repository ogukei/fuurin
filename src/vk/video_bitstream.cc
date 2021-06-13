
#include "vk/video_bitstream.h"

#include <cstring>

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"
#include "vk/command_pool.h"

#include "vk/video_bitstream_segment.h"

namespace vk {

std::shared_ptr<VideoBitstreamBuffer> VideoBitstreamBuffer::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool) {
  auto video_bitstream_buffer = std::make_shared<VideoBitstreamBuffer>(command_pool);
  video_bitstream_buffer->Initialize();
  return video_bitstream_buffer;
}

VideoBitstreamBuffer::VideoBitstreamBuffer(
    const std::shared_ptr<vk::CommandPool>& command_pool)
    : command_pool_(command_pool),
      offset_(0) {
}

void VideoBitstreamBuffer::Initialize() {
  buffer_memory_ = BufferMemory::Create(
    command_pool_->Device(),
    command_pool_->Queue(),
    1024 * 1024 * 8,
    VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    .value();
}

void VideoBitstreamBuffer::Write(const void* data, const VideoBitstreamSegmentReference& segment_reference) {
  auto& device = command_pool_->Device();
  void *mapped = nullptr;
  // mapping
  vkMapMemory(device->Handle(), buffer_memory_->Memory(),
    segment_reference.offset,
    segment_reference.region_size,
    0,
    &mapped);
  std::memcpy(mapped, data, segment_reference.size);
  {
    VkMappedMemoryRange range = {
      .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
      .pNext = nullptr,
      .memory = buffer_memory_->Memory(),
      .offset = segment_reference.offset,
      .size = segment_reference.region_size,
    };
    vkFlushMappedMemoryRanges(device->Handle(), 1, &range);
  }
  vkUnmapMemory(device->Handle(), buffer_memory_->Memory());
}

void VideoBitstreamBuffer::AppendSegment(const vk::VideoSliceLayerBitstreamSegment& segment) {
  // align mapped region with atomic size
  const size_t alignment = 0x40;
  size_t region_size = ((segment.size + (alignment - 1)) & ~(alignment - 1));
  VideoBitstreamSegmentReference segment_reference = {
    .region_size = region_size,
    .size = segment.size,
    .offset = 0,
  };
  Write(segment.data, segment_reference);
  segment_references_.push_back(segment_reference);
  // calculate next offset
  // offset_ += region_size;
}

std::optional<VideoBitstreamSegmentReference> VideoBitstreamBuffer::PopFrontSegmentReference() {
  if (segment_references_.empty()) return std::nullopt;
  VideoBitstreamSegmentReference reference = segment_references_.front();
  segment_references_.pop_front();
  return reference;
}

VkBuffer VideoBitstreamBuffer::Buffer() const {
  return buffer_memory_->Buffer();
}

VideoBitstreamBuffer::~VideoBitstreamBuffer() {
}

}  // namespace vk
