
#include "vk/video_query_pool.h"

#include <iostream>
#include <cassert>

#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/queue.h"
#include "vk/video_profile.h"

namespace vk {

std::optional<std::shared_ptr<VideoQueryPool>> VideoQueryPool::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t query_count) {
  auto query_pool = std::make_shared<VideoQueryPool>(device, video_profile, query_count);
  return (query_pool->Initialize()) ? std::optional {query_pool} : std::nullopt;
}

VideoQueryPool::VideoQueryPool(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t query_count)
    : device_(device),
      video_profile_(video_profile),
      query_count_(query_count),
      query_pool_(nullptr) {
}

bool VideoQueryPool::Initialize() {
  auto& device = device_;
  VkQueryPoolCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
    .pNext = &video_profile_->ProfileWithExt(),
    .flags = 0,
    .queryType = VK_QUERY_TYPE_RESULT_STATUS_ONLY_KHR,
    .queryCount = query_count_,
    .pipelineStatistics = 0
  };
  VkQueryPool query_pool = nullptr;
  vkCreateQueryPool(device->Handle(), &create_info, nullptr, &query_pool);
  if (query_pool == nullptr) return false;
  query_pool_ = query_pool;
  return true;
}

void VideoQueryPool::DumpResult() {
  struct NvVideoGetDecodeStatus {
    VkQueryResultStatusKHR decodeStatus;
    uint32_t hwCyclesCount;
    uint32_t hwStatus;
    uint32_t mbsCorrectlyDecoded;
    uint32_t mbsInError;
    uint16_t instanceId;
    uint16_t reserved1;
  };
  NvVideoGetDecodeStatus decode_status = {};
  VkResult result = vkGetQueryPoolResults(device_->Handle(),
    Handle(), 0, 1, sizeof(decode_status), &decode_status, 512, VK_QUERY_RESULT_WAIT_BIT);
  assert(result == VK_SUCCESS);
  std::cout << "VideoQueryPool::DumpResult()" << std::endl
    << "  status: " << decode_status.decodeStatus << std::endl
    << "  cycles_count: " << decode_status.hwCyclesCount << std::endl
    << "  hw_status: " << decode_status.hwStatus << std::endl
    << "  correctly_decoded: " << decode_status.mbsCorrectlyDecoded << std::endl
    << "  in_error: " << decode_status.mbsInError << std::endl
    << "  instance_id: " << decode_status.instanceId << std::endl;
  assert(decode_status.decodeStatus == VK_QUERY_RESULT_STATUS_COMPLETE_KHR);
}

VideoQueryPool::~VideoQueryPool() {
  auto& device = device_;
  vkDestroyQueryPool(device->Handle(), query_pool_, nullptr);
  query_pool_ = nullptr;
}

}  // namespace vk
