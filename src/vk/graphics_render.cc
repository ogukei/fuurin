
#include "vk/graphics_render.h"

#include <array>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vk/device.h"
#include "vk/queue.h"
#include "vk/device_queue.h"
#include "vk/command_pool.h"
#include "vk/command_buffer.h"
#include "vk/command_record.h"
#include "vk/render_pass.h"
#include "vk/framebuffer.h"
#include "vk/graphics_pipeline.h"
#include "vk/graphics_state.h"

namespace vk {

std::shared_ptr<GraphicsRender> GraphicsRender::Create(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::GraphicsPipeline>& pipeline,
    const std::shared_ptr<vk::GraphicsState>& state) {
  auto graphics_render = std::make_shared<GraphicsRender>(command_pool, pipeline, state);
  graphics_render->Initialize();
  return graphics_render;
}

GraphicsRender::GraphicsRender(
    const std::shared_ptr<vk::CommandPool>& command_pool,
    const std::shared_ptr<vk::GraphicsPipeline>& pipeline,
    const std::shared_ptr<vk::GraphicsState>& state)
    : command_pool_(command_pool),
      pipeline_(pipeline),
      state_(state) {
}

void GraphicsRender::Initialize() {
  auto& framebuffer = pipeline_->Framebuffer();
  auto command_record = CommandRecord::Begin(command_pool_)
    .value();
  // clear
  std::array<VkClearValue, 2> clear_values = {
    VkClearValue {
      .color = { { 0.0f, 0.0f, 0.2f, 1.0f } }
    },
    VkClearValue {
      .depthStencil = { 1.0f, 0 }
    }
  };
  // begin info
  VkRenderPassBeginInfo render_pass_begin_info = {
    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    .pNext = nullptr,
    .renderPass = framebuffer->RenderPass()->Handle(),
    .framebuffer = framebuffer->Handle(),
    .renderArea = VkRect2D {
      .offset = {},
      .extent = VkExtent2D {
        .width = framebuffer->Width(),
        .height = framebuffer->Height()
      }
    },
    .clearValueCount = (uint32_t)clear_values.size(),
    .pClearValues = clear_values.data(),
  };
  vkCmdBeginRenderPass(command_record->CommandBuffer(), &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  // viewport
  VkViewport viewport = {
    .x = 0.0f,
    .y = 0.0f,
    .width = static_cast<float>(framebuffer->Width()),
    .height = static_cast<float>(framebuffer->Height()),
    .minDepth = 0.0f,
    .maxDepth = 1.0f
  };
  vkCmdSetViewport(command_record->CommandBuffer(), 0, 1, &viewport);
  // scissor
  VkRect2D scissor = {
    .offset = {},
    .extent = VkExtent2D {
      .width = framebuffer->Width(),
      .height = framebuffer->Height()
    }
  };
  vkCmdSetScissor(command_record->CommandBuffer(), 0, 1, &scissor);
  // pipeline
  vkCmdBindPipeline(command_record->CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->Handle());
  // vertex
  std::array<VkDeviceSize, 1> offsets = {0};
  VkBuffer vertex_buffer = state_->VertexBuffer();
  vkCmdBindVertexBuffers(command_record->CommandBuffer(), 0, 1, &vertex_buffer, offsets.data());
  // index
  VkBuffer index_buffer = state_->IndexBuffer();
  vkCmdBindIndexBuffer(command_record->CommandBuffer(), index_buffer, 0, VK_INDEX_TYPE_UINT32);
  // draw
  std::vector<glm::vec3> positions = {
    glm::vec3(-1.5f, 0.0f, -4.0f),
    glm::vec3(+0.0f, 0.0f, -2.5f),
    glm::vec3(+1.5f, 0.0f, -4.0f)
  };
  for (auto& v : positions) {
    float width = static_cast<float>(framebuffer->Width());
    float height = static_cast<float>(framebuffer->Height());
    glm::mat4 matrix = glm::perspective(
      glm::radians(60.0f), width / height, 0.1f, 256.0f) * glm::translate(glm::mat4(1.0f), v);
    vkCmdPushConstants(
      command_record->CommandBuffer(),
      pipeline_->Layout(),
      VK_SHADER_STAGE_VERTEX_BIT,
      0,
      sizeof(glm::mat4),
      &matrix);
    vkCmdDrawIndexed(command_record->CommandBuffer(), 3, 1, 0, 0, 0);
  }
  vkCmdEndRenderPass(command_record->CommandBuffer());
  // finish
  command_buffer_ = command_record->End();
}

void GraphicsRender::Render() {
  auto& queue = command_pool_->DeviceQueue()->Queue();
  queue->SubmitThenWait(command_buffer_);
}

GraphicsRender::~GraphicsRender() {
}

}  // namespace vk
