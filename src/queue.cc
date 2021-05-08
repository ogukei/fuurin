
#include "queue.h"

#include "device_queue.h"

namespace vk {

Queue::Queue(const std::shared_ptr<DeviceQueue>& device_queue) : queue_(nullptr), device_queue_(device_queue) {

}

uint32_t Queue::FamilyIndex() const {
  return device_queue_->QueueFamilyIndex();
}

void Queue::Initialize(VkQueue queue) {
  queue_ = queue;
}

} // namespace vk
