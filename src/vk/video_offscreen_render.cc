
#include "vk/video_offscreen_render.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/device_queue.h"
#include "vk/command_pool.h"
#include "vk/command_record.h"
#include "vk/image.h"
#include "vk/image_memory.h"
#include "vk/video_frame.h"
#include "vk/video_image.h"

namespace vk {

std::shared_ptr<VideoOffscreenRender> VideoOffscreenRender::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::VideoSessionFrame>& frame) {
  auto instance = std::make_shared<VideoOffscreenRender>(command_pool, frame);
  instance->Initialize();
  return instance;
}

VideoOffscreenRender::VideoOffscreenRender(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::VideoSessionFrame>& frame)
    : command_pool_(command_pool),
      frame_(frame),
      mapped_data_(nullptr) {
}

void VideoOffscreenRender::Initialize() {
  auto width = frame_->Width();
  auto height = frame_->Height();
  auto& device = command_pool_->Device();
  auto image = Image::Create(
    device,
    width,
    height,
    VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
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
        .aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT,
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
  VkImageCopy plane0_copy = {
    .srcSubresource = VkImageSubresourceLayers {
      .aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcOffset = {},
    .dstSubresource = VkImageSubresourceLayers {
      .aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT,
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
  VkImageCopy plane1_copy = {
    .srcSubresource = VkImageSubresourceLayers {
      .aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcOffset = {},
    .dstSubresource = VkImageSubresourceLayers {
      .aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .dstOffset = {},
    .extent = VkExtent3D {
      .width = width / 2,
      .height = height / 2,
      .depth = 1,
    },
  };
  std::vector<VkImageCopy> copies = {plane0_copy, plane1_copy};
  vkCmdCopyImage(
      command_record->CommandBuffer(),
      frame_->VideoImage()->Handle(),
      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
      image->Handle(),
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      static_cast<uint32_t>(copies.size()),
      copies.data());
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
        .aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_0_BIT,
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
  {
    VkImageSubresource subresource = {
      .aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT,
      .mipLevel = 0,
      .arrayLayer = 0,
    };
    VkSubresourceLayout subresource_layout = {};
    vkGetImageSubresourceLayout(device->Handle(), image->Handle(), &subresource, &subresource_layout);
    plane0_layout_ = subresource_layout;
  }
  {
    VkImageSubresource subresource = {
      .aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT,
      .mipLevel = 0,
      .arrayLayer = 0,
    };
    VkSubresourceLayout subresource_layout = {};
    vkGetImageSubresourceLayout(device->Handle(), image->Handle(), &subresource, &subresource_layout);
    plane1_layout_ = subresource_layout;
  }
  image_ = image;
  image_memory_ = image_memory;
  // mapping
  void* data = nullptr;
  vkMapMemory(device->Handle(), image_memory->Handle(), 0, VK_WHOLE_SIZE, 0, &data);
  uint8_t* bytes = reinterpret_cast<uint8_t*>(data);
  mapped_data_ = reinterpret_cast<void *>(bytes);
}

void VideoOffscreenRender::Execute() {
  auto& queue = command_pool_->Queue();
  queue->SubmitThenWait(command_buffer_);
}

static void yuv_to_rgb(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t y, uint8_t u, uint8_t v) {
  int32_t ir = y + (1.370705f * (v - 128.0f));
  int32_t ig = y - (0.698001f * (v - 128.0f)) - (0.337633f * (u - 128.0f));
  int32_t ib = y + (1.732446f * (u - 128.0f));
  *r = std::clamp(ir, 0, 255);
  *g = std::clamp(ig, 0, 255);
  *b = std::clamp(ib, 0, 255);
}

void VideoOffscreenRender::Save(const std::string& filename) {
  auto width = frame_->Width();
  auto height = frame_->Height();
  // .ppm
  std::ofstream output(filename, std::ios::out | std::ios::binary);
  output << "P6\n" << width << "\n" << height << "\n" << 255 << "\n";
  // YUV420 multi-planar
  // VK_FORMAT_G8_B8R8_2PLANE_420_UNORM specifies an unsigned normalized multi-planar format that
  // has an 8-bit G component in plane 0
  // and a two-component, 16-bit BR plane 1 consisting of an 8-bit B component
  // in byte 0 and an 8-bit R component in byte 1.
  // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkFormat.html
  uint8_t* components = reinterpret_cast<uint8_t*>(mapped_data_);
  uint8_t* g_components = components + plane0_layout_->offset;
  uint16_t* br_components = reinterpret_cast<uint16_t*>(components + plane1_layout_->offset);
  for (int32_t y = 0; y < height; y++) {
    for (int32_t x = 0; x < width; x++) {
      int32_t i_g = y * width + x;
      int32_t i_br = (y / 2) * (width / 2) + (x / 2);
      uint8_t* g_ptr = g_components + i_g;
      uint8_t* br_ptr = reinterpret_cast<uint8_t*>(br_components + i_br);
      uint8_t y = *g_ptr;
      uint8_t u = *br_ptr;
      uint8_t v = *(br_ptr + 1);
      uint8_t r, g, b;
      yuv_to_rgb(&r, &g, &b, y, u, v);
      output.write(reinterpret_cast<char*>(&r), 1);
      output.write(reinterpret_cast<char*>(&g), 1);
      output.write(reinterpret_cast<char*>(&b), 1);
    }
  }
  std::cout << "Image successfully written to " << filename << std::endl;
}

VideoOffscreenRender::~VideoOffscreenRender() {
  auto& device = command_pool_->Device();
  if (mapped_data_ != nullptr) {
    vkUnmapMemory(device->Handle(), image_memory_->Handle());
  }
}

}  // namespace vk
