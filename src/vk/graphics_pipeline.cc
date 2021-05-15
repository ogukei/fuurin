
#include "vk/graphics_pipeline.h"

#include <vector>

#include "vk/device.h"
#include "vk/framebuffer.h"
#include "vk/render_pass.h"
#include "vk/shader_module.h"

namespace vk {

std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Framebuffer>& framebuffer) {
  auto graphics_pipeline = std::make_shared<GraphicsPipeline>(device, framebuffer);
  graphics_pipeline->Initialize();
  return graphics_pipeline;
}

GraphicsPipeline::GraphicsPipeline(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Framebuffer>& framebuffer)
    : device_(device),
      framebuffer_(framebuffer),
      pipeline_(nullptr),
      descriptor_set_layout_(nullptr),
      pipeline_layout_(nullptr),
      pipeline_cache_(nullptr) {
}

void GraphicsPipeline::Initialize() {
  VkDescriptorSetLayout descriptor_set_layout = nullptr;
  {
    std::vector<VkDescriptorSetLayoutBinding> bindings = {};
    VkDescriptorSetLayoutCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .bindingCount = (uint32_t)bindings.size(),
      .pBindings = bindings.data(),
    };
    vkCreateDescriptorSetLayout(device_->Handle(), &create_info, nullptr, &descriptor_set_layout);
    descriptor_set_layout_ = descriptor_set_layout;
  }
  //
  VkPipelineLayout pipeline_layout = nullptr;
  {
    std::vector<VkPushConstantRange> push_constant_ranges = {
      VkPushConstantRange {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(float) * 16,  // 4x4 matrix
      }
    };
    VkPipelineLayoutCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = 1,
      .pSetLayouts = &descriptor_set_layout,
      .pushConstantRangeCount = (uint32_t)push_constant_ranges.size(),
      .pPushConstantRanges = push_constant_ranges.data(),
    };
    vkCreatePipelineLayout(device_->Handle(), &create_info, nullptr, &pipeline_layout);
    pipeline_layout_ = pipeline_layout;
  }
  //
  VkPipelineCache pipeline_cache = nullptr;
  {
    VkPipelineCacheCreateInfo create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .initialDataSize = 0,
      .pInitialData = nullptr,
    };
    vkCreatePipelineCache(device_->Handle(), &create_info, nullptr, &pipeline_cache);
    pipeline_cache_ = pipeline_cache;
  }
  //
  VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };
  // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPipelineRasterizationStateCreateInfo.html
  VkPipelineRasterizationStateCreateInfo rasterization_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.0f,
    .depthBiasClamp = 0.0f,
    .depthBiasSlopeFactor = 0.0f,
    .lineWidth = 1.0f,
  };
  VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
    .blendEnable = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkColorComponentFlagBits.html
    // rgba
    .colorWriteMask = 0xf,
  };
  VkPipelineColorBlendStateCreateInfo color_blend_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_CLEAR,
    .attachmentCount = 1,
    .pAttachments = &color_blend_attachment_state,
    .blendConstants = {0.0f},
  };
  VkPipelineViewportStateCreateInfo viewport_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = nullptr,
    .scissorCount = 1,
    .pScissors = nullptr,
  };
  std::vector<VkDynamicState> dynamic_states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };
  VkPipelineDynamicStateCreateInfo dynamic_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .dynamicStateCount = (uint32_t)dynamic_states.size(),
    .pDynamicStates = dynamic_states.data(),
  };
  // depth stencil state
  VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE,
    .front = VkStencilOpState {
      .failOp = VK_STENCIL_OP_KEEP,
      .passOp = VK_STENCIL_OP_KEEP,
      .depthFailOp = VK_STENCIL_OP_KEEP,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .compareMask = 0,
      .writeMask = 0,
      .reference = 0,
    },
    .back = VkStencilOpState {
      .failOp = VK_STENCIL_OP_KEEP,
      .passOp = VK_STENCIL_OP_KEEP,
      .depthFailOp = VK_STENCIL_OP_KEEP,
      .compareOp = VK_COMPARE_OP_ALWAYS,
      .compareMask = 0,
      .writeMask = 0,
      .reference = 0,
    },
    .minDepthBounds = 0.0f,
    .maxDepthBounds = 0.0f,
  };
  // multisample
  VkPipelineMultisampleStateCreateInfo multisample_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0.0f,
    .pSampleMask = nullptr,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE,
  };
  // vertex input bindings
  std::vector<VkVertexInputBindingDescription> vertex_input_bindings = {
    VkVertexInputBindingDescription {
      .binding = 0,
      .stride = sizeof(float) * 6,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    }
  };
  // vertex input attributes
  std::vector<VkVertexInputAttributeDescription> vertex_input_attributes = {
    // position
    VkVertexInputAttributeDescription {
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = 0,
    },
    // color
    VkVertexInputAttributeDescription {
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = sizeof(float) * 3,
    }
  };
  // vertex input
  VkPipelineVertexInputStateCreateInfo vertex_input_state = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .vertexBindingDescriptionCount = (uint32_t)vertex_input_bindings.size(),
    .pVertexBindingDescriptions = vertex_input_bindings.data(),
    .vertexAttributeDescriptionCount = (uint32_t)vertex_input_attributes.size(),
    .pVertexAttributeDescriptions = vertex_input_attributes.data(),
  };
  // shaders
  auto vertex_shader_module = vk::ShaderModule::Create(device_, "data/shaders/raster.triangle.vert.spv")
    .value();
  auto fragment_shader_module = vk::ShaderModule::Create(device_, "data/shaders/raster.triangle.frag.spv")
    .value();
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
    VkPipelineShaderStageCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = vertex_shader_module->Handle(),
      .pName = "main",
      .pSpecializationInfo = nullptr,
    },
    VkPipelineShaderStageCreateInfo {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = fragment_shader_module->Handle(),
      .pName = "main",
      .pSpecializationInfo = nullptr,
    },
  };
  // @see https://khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkGraphicsPipelineCreateInfo.html
  VkGraphicsPipelineCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .stageCount = (uint32_t)shader_stages.size(),
    .pStages = shader_stages.data(),
    .pVertexInputState = &vertex_input_state,
    .pInputAssemblyState = &input_assembly_state,
    .pTessellationState = nullptr,
    .pViewportState = &viewport_state,
    .pRasterizationState = &rasterization_state,
    .pMultisampleState = &multisample_state,
    .pDepthStencilState = &depth_stencil_state,
    .pColorBlendState = &color_blend_state,
    .pDynamicState = &dynamic_state,
    .layout = pipeline_layout,
    .renderPass = framebuffer_->RenderPass()->Handle(),
    .subpass = 0,
    .basePipelineHandle = nullptr,
    .basePipelineIndex = 0,
  };
  VkPipeline pipeline = nullptr;
  vkCreateGraphicsPipelines(device_->Handle(), pipeline_cache, 1, &create_info, nullptr, &pipeline);
  pipeline_ = pipeline;
}

GraphicsPipeline::~GraphicsPipeline() {
  vkDestroyDescriptorSetLayout(device_->Handle(), descriptor_set_layout_, nullptr);
  descriptor_set_layout_ = nullptr;
  vkDestroyPipelineLayout(device_->Handle(), pipeline_layout_, nullptr);
  pipeline_layout_ = nullptr;
  vkDestroyPipelineCache(device_->Handle(), pipeline_cache_, nullptr);
  pipeline_cache_ = nullptr;
  vkDestroyPipeline(device_->Handle(), pipeline_, nullptr);
  pipeline_ = nullptr;
}

}  // namespace vk
