
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

class QueryPool {
 private:
  std::shared_ptr<vk::Device> device_;
  VkQueryType query_type_;
  uint32_t query_count_;
  VkQueryPool query_pool_;

  bool Initialize();

 public:
  static std::optional<std::shared_ptr<QueryPool>> Create(
    const std::shared_ptr<vk::Device>& device,
    VkQueryType query_type,
    uint32_t query_count);

  explicit QueryPool(
    const std::shared_ptr<vk::Device>& device,
    VkQueryType query_type,
    uint32_t query_count);

  QueryPool(const QueryPool &) = delete;
  ~QueryPool();

  VkQueryPool Handle() const { return query_pool_; }

  const std::shared_ptr<vk::Device>& Device() const { return device_; }
};

}  // namespace vk
