
#include "vk/offscreen_render.h"

#include <fstream>

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/device_queue.h"
#include "vk/command_pool.h"
#include "vk/command_record.h"
#include "vk/image.h"
#include "vk/image_memory.h"
#include "vk/framebuffer.h"

namespace vk {

std::shared_ptr<OffscreenRender> OffscreenRender::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::Framebuffer>& framebuffer) {
  auto instance = std::make_shared<OffscreenRender>(command_pool, framebuffer);
  instance->Initialize();
  return instance;
}

OffscreenRender::OffscreenRender(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::Framebuffer>& framebuffer)
    : command_pool_(command_pool),
      framebuffer_(framebuffer),
      mapped_data_(nullptr) {
}

void OffscreenRender::Initialize() {
  auto width = framebuffer_->Width();
  auto height = framebuffer_->Height();
  auto& device = framebuffer_->Device();
  auto image = Image::Create(
    device,
    width,
    height,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    VK_IMAGE_TILING_LINEAR);
  auto image_memory = ImageMemory::Create(
    device,
    image->Handle(),
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  auto command_record = CommandRecord::Begin(command_pool_)
    .value();
  // barrier
  {
    VkImageMemoryBarrier memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = nullptr,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image->Handle(),
      .subresourceRange = VkImageSubresourceRange {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };
    vkCmdPipelineBarrier(
      command_record->CommandBuffer(),
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &memory_barrier);
  }
  //
  VkImageCopy image_copy = {
    .srcSubresource = VkImageSubresourceLayers {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcOffset = {},
    .dstSubresource = VkImageSubresourceLayers {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .dstOffset = {},
    .extent = VkExtent3D {
      .width = width,
      .height = height,
      .depth = 1,
    },
  };
  vkCmdCopyImage(
      command_record->CommandBuffer(),
      framebuffer_->ColorImage()->Handle(),
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      image->Handle(),
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1,
      &image_copy);
  // barrier
  {
    VkImageMemoryBarrier memory_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = nullptr,
      .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image->Handle(),
      .subresourceRange = VkImageSubresourceRange {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      },
    };
    vkCmdPipelineBarrier(
      command_record->CommandBuffer(),
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &memory_barrier);
  }
  command_buffer_ = command_record->End();
  //
  VkImageSubresource subresource = {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .mipLevel = 0,
    .arrayLayer = 0,
  };
  VkSubresourceLayout subresource_layout = {};
  vkGetImageSubresourceLayout(device->Handle(), image->Handle(), &subresource, &subresource_layout);
  layout_ = subresource_layout;
  image_ = image;
  image_memory_ = image_memory;
  // mapping
  void* data = nullptr;
  vkMapMemory(device->Handle(), image_memory->Handle(), 0, VK_WHOLE_SIZE, 0, &data);
  uint8_t* bytes = reinterpret_cast<uint8_t*>(data);
  bytes += subresource_layout.offset;
  mapped_data_ = reinterpret_cast<void *>(bytes);
}

void OffscreenRender::Execute() {
  auto& queue = command_pool_->Queue();
  queue->SubmitThenWait(command_buffer_);
}

void OffscreenRender::Save(const std::string& filename) {
  auto width = framebuffer_->Width();
  auto height = framebuffer_->Height();
  size_t row_pitch = static_cast<size_t>(layout_.value().rowPitch);
  // .ppm
  std::ofstream output(filename, std::ios::out | std::ios::binary);
  output << "P6\n" << width << "\n" << height << "\n" << 255 << "\n";
  uint32_t* components = reinterpret_cast<uint32_t*>(mapped_data_);
  for (int32_t y = 0; y < height; y++) {
    uint32_t* row = components;
    for (int32_t x = 0; x < width; x++) {
      output.write(reinterpret_cast<char*>(row), 3);
      row++;
    }
    components += row_pitch / 4;
  }
}

OffscreenRender::~OffscreenRender() {
  auto& device = framebuffer_->Device();
  if (mapped_data_ != nullptr) {
    vkUnmapMemory(device->Handle(), image_memory_->Handle());
  }
}

}  // namespace vk
