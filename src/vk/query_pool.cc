
#include "vk/query_pool.h"

#include "vk/device_queue.h"
#include "vk/device.h"
#include "vk/queue.h"

namespace vk {

std::optional<std::shared_ptr<QueryPool>> QueryPool::Create(
    const std::shared_ptr<vk::Device>& device,
    VkQueryType query_type,
    uint32_t query_count) {
  auto query_pool = std::make_shared<QueryPool>(device, query_type, query_count);
  return (query_pool->Initialize()) ? std::optional {query_pool} : std::nullopt;
}

QueryPool::QueryPool(
    const std::shared_ptr<vk::Device>& device,
    VkQueryType query_type,
    uint32_t query_count)
    : device_(device),
      query_type_(query_type),
      query_count_(query_count),
      query_pool_(nullptr) {
}

bool QueryPool::Initialize() {
  auto& device = device_;
  VkQueryPoolCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queryType = query_type_,
    .queryCount = query_count_,
    .pipelineStatistics = 0
  };
  VkQueryPool query_pool = nullptr;
  vkCreateQueryPool(device->Handle(), &create_info, nullptr, &query_pool);
  if (query_pool == nullptr) return false;
  query_pool_ = query_pool;
  return true;
}

QueryPool::~QueryPool() {
  auto& device = device_;
  vkDestroyQueryPool(device->Handle(), query_pool_, nullptr);
  query_pool_ = nullptr;
}

}  // namespace vk
