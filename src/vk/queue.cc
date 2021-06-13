
#include "vk/queue.h"

#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/command_buffer.h"
#include "vk/command_pool.h"

namespace vk {

Queue::Queue(const std::shared_ptr<DeviceQueue>& device_queue, uint32_t family_index)
    : queue_(nullptr), device_queue_(device_queue), family_index_(family_index) {
}

void Queue::Initialize(VkQueue queue) {
  queue_ = queue;
}

void Queue::Submit(const std::shared_ptr<CommandBuffer>& command_buffer) {
  auto& device = command_buffer->CommandPool()->Device();
  VkCommandBuffer command_buffer_handle = command_buffer->Handle();

  // FIXME:
  VkPipelineStageFlags wait_dst = VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR;

  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = nullptr,
    .pWaitDstStageMask = &wait_dst,
    .commandBufferCount = 1,
    .pCommandBuffers = &command_buffer_handle,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = nullptr
  };
  VkFenceCreateInfo fence_info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0
  };
  VkFence fence = nullptr;
  vkCreateFence(device->Handle(), &fence_info, nullptr, &fence);
  vkQueueSubmit(queue_, 1, &submit_info, fence);
  vkWaitForFences(device->Handle(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(device->Handle(), fence, nullptr);
}

void Queue::SubmitThenWait(const std::shared_ptr<CommandBuffer>& command_buffer) {
  auto& device = command_buffer->CommandPool()->Device();
  VkCommandBuffer command_buffer_handle = command_buffer->Handle();
  VkSubmitInfo submit_info = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = nullptr,
    .pWaitDstStageMask = nullptr,
    .commandBufferCount = 1,
    .pCommandBuffers = &command_buffer_handle,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = nullptr
  };
  VkFenceCreateInfo fence_info = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0
  };
  VkFence fence = nullptr;
  vkCreateFence(device->Handle(), &fence_info, nullptr, &fence);
  vkQueueSubmit(queue_, 1, &submit_info, fence);
  vkWaitForFences(device->Handle(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkDestroyFence(device->Handle(), fence, nullptr);
}

}  // namespace vk
