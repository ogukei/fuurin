
#include "vk/video_memories.h"

#include <utility>
#include <iostream>
#include <cstring>

#include "vk/buffer_memory.h"
#include "vk/device.h"
#include "vk/device_queue.h"
#include "vk/physical_device.h"
#include "vk/video_memory.h"

namespace vk {

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkGetVideoSessionMemoryRequirementsKHR.html
static PFN_vkGetVideoSessionMemoryRequirementsKHR vk_vkGetVideoSessionMemoryRequirementsKHR(VkDevice device) {
    return (PFN_vkGetVideoSessionMemoryRequirementsKHR)vkGetDeviceProcAddr(
      device, "vkGetVideoSessionMemoryRequirementsKHR");
}

// @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkBindVideoSessionMemoryKHR.html
static PFN_vkBindVideoSessionMemoryKHR vk_vkBindVideoSessionMemoryKHR(VkDevice device) {
    return (PFN_vkBindVideoSessionMemoryKHR)vkGetDeviceProcAddr(
      device, "vkBindVideoSessionMemoryKHR");
}

std::shared_ptr<VideoSessionMemories> VideoSessionMemories::Create(
    const std::shared_ptr<vk::Device>& device,
    VkVideoSessionKHR video_session) {
  auto memories = std::make_shared<VideoSessionMemories>(device, video_session);
  memories->Initialize();
  return memories;
}

VideoSessionMemories::VideoSessionMemories(
    const std::shared_ptr<vk::Device>& device,
    VkVideoSessionKHR video_session)
    : device_(device),
      video_session_(video_session) {
}

void VideoSessionMemories::Initialize() {
  auto& device = device_;
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/libs/NvVkDecoder/NvVkDecoder.cpp
  // first, retrieves required count
  uint32_t video_requirements_count = 0;
  vk_vkGetVideoSessionMemoryRequirementsKHR(device->Handle())(
    device->Handle(), video_session_, &video_requirements_count, nullptr);
  // allocate
  std::vector<VkVideoGetMemoryPropertiesKHR> video_properties_vec;
  std::vector<VkMemoryRequirements2> memory_requirements_vec;
  video_properties_vec.resize(video_requirements_count);
  memory_requirements_vec.resize(video_requirements_count);
  // associate
  for (uint32_t i = 0; i < video_requirements_count; i++) {
    VkMemoryRequirements2* requirements = memory_requirements_vec.data() + (size_t)i;
    VkVideoGetMemoryPropertiesKHR* properties = video_properties_vec.data() + (size_t)i;
    requirements->sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
    properties->sType = VK_STRUCTURE_TYPE_VIDEO_GET_MEMORY_PROPERTIES_KHR;
    properties->pMemoryRequirements = requirements;
  }
  vk_vkGetVideoSessionMemoryRequirementsKHR(device->Handle())(
    device->Handle(), video_session_, &video_requirements_count, video_properties_vec.data());
  // fill memories
  auto& memories = memories_;
  memories.resize(video_requirements_count);
  for (uint32_t i = 0; i < video_requirements_count; i++) {
    VkMemoryRequirements2* memory_requirements = memory_requirements_vec.data() + (size_t)i;
    auto memory = VideoSessionMemory::Create(device_,
      memory_requirements->memoryRequirements.size,
      memory_requirements->memoryRequirements.memoryTypeBits);
    memories[i] = std::move(memory);
  }
  // binding
  std::vector<VkVideoBindMemoryKHR> bind_memory_vec;
  bind_memory_vec.resize(video_requirements_count);
  for (uint32_t i = 0; i < video_requirements_count; i++) {
    VkMemoryRequirements2* memory_requirements = memory_requirements_vec.data() + (size_t)i;
    VkVideoGetMemoryPropertiesKHR* properties = video_properties_vec.data() + (size_t)i;
    auto& memory = memories.at(i);
    VkVideoBindMemoryKHR bind_memory = {
      .sType = VK_STRUCTURE_TYPE_VIDEO_BIND_MEMORY_KHR,
      .pNext = nullptr,
      .memoryBindIndex = properties->memoryBindIndex,
      .memory = memory->Handle(),
      .memoryOffset = 0,
      .memorySize = memory_requirements->memoryRequirements.size,
    };
    bind_memory_vec[i] = bind_memory;
  }
  vk_vkBindVideoSessionMemoryKHR(device->Handle())(
    device->Handle(),
    video_session_,
    static_cast<uint32_t>(bind_memory_vec.size()),
    bind_memory_vec.data());
}

VideoSessionMemories::~VideoSessionMemories() {
}

}  // namespace vk
