
#include "vk/video_bitstream.h"

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"

namespace vk {

std::shared_ptr<VideoBitstreamBuffer> VideoBitstreamBuffer::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool) {
  auto video_bitstream_buffer = std::make_shared<VideoBitstreamBuffer>(command_pool);
  video_bitstream_buffer->Initialize();
  return video_bitstream_buffer;
}

VideoBitstreamBuffer::VideoBitstreamBuffer(
    const std::shared_ptr<vk::CommandPool>& command_pool)
    : command_pool_(command_pool) {
}

void VideoBitstreamBuffer::Initialize() {
}

VideoBitstreamBuffer::~VideoBitstreamBuffer() {
}

}  // namespace vk
