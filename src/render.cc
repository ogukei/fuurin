
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
#include "render.h"

extern "C" {
#include <stdio.h>
}

#include <vector>
#include <string>
#include <optional>

#define VK_MAKE_VIDEO_STD_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

// @see https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h#L5
#define VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9 VK_MAKE_VIDEO_STD_VERSION(0, 9, 0) // Patch version should always be set to 0

// Format must be in the form XX.XX where the first two digits are the major and the second two, the minor.
#define VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION   VK_STD_VULKAN_VIDEO_CODEC_H264_API_VERSION_0_9
#define VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME "VK_STD_vulkan_video_codec_h264"

static PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(VkInstance instance) {
  return (PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR");
}

Render::Render() {
  // instance
  VkApplicationInfo app_info = {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Fuurin";
  app_info.pEngineName = "Fuurin";
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo instance_info = {};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;

  VkInstance instance = nullptr;
  vkCreateInstance(&instance_info, nullptr, &instance);

  // physical device
  VkPhysicalDevice physical_device = nullptr;
  {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    std::vector<VkPhysicalDevice> devices;
    devices.resize(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
    physical_device = devices[0];
  }
  {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    printf("GPU: %s\n", properties.deviceName);
  }
  // queue family
  std::optional<uint32_t> queue_family_index;
  {
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, nullptr);
    std::vector<VkQueueFamilyProperties> families;
    families.resize(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_count, families.data());
    {
      for (uint32_t i = 0; i < family_count; i++) {
        const VkQueueFamilyProperties& properties = families.at(i);
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
          queue_family_index = i;
          break;
        }
      }
    }
  }
  // queue & device
  VkDevice device = nullptr;
  {
    const float queue_priority = 0.0f;
    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueCount = 1;
    queue_info.queueFamilyIndex = queue_family_index.value();
    queue_info.pQueuePriorities = &queue_priority;
    // logical device
    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    //
    std::vector<const char*> extension_names = {
      "VK_EXT_video_decode_h264"
    };
    device_info.enabledExtensionCount = 1;
    device_info.ppEnabledExtensionNames = extension_names.data();
    vkCreateDevice(physical_device, &device_info, nullptr, &device);
  }
  auto func = vk_vkGetPhysicalDeviceVideoCapabilitiesKHR(instance);
  VkVideoCapabilitiesKHR video_capabilities = { VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR, nullptr };

  // @see PopulateProfileExt 
  // https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/nvVideoProfile.h#L55
  VkVideoProfileKHR profile = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_PROFILE_KHR,
    .pNext = nullptr,
    .videoCodecOperation = VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT,
    // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoChromaSubsamplingFlagBitsKHR.html
    .chromaSubsampling = VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR,
    // @see https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkVideoComponentBitDepthFlagBitsKHR.html
    .lumaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
    .chromaBitDepth = VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR,
  };
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/058d03f6c432053ee3e7428f3f543be90eec947c/vk_video_decoder/libs/VkCodecUtils/nvVideoProfile.h#L55
  VkVideoDecodeH264ProfileEXT decode_h264_profile = {
    .sType = VK_STRUCTURE_TYPE_VIDEO_DECODE_H264_PROFILE_EXT,
    .pNext = nullptr,
    // note that `std_video_h264_profile_idc_main` is defined in vulkan_video_codec_h264std.h
    // @see https://github.com/nvpro-samples/vk_video_samples/blob/c851b02743574def866c593fe66d1dff93354e6d/vk_video_decoder/include/vk_video/vulkan_video_codec_h264std.h
    .stdProfileIdc = std_video_h264_profile_idc_high,
    .fieldLayout = VK_VIDEO_DECODE_H264_FIELD_LAYOUT_LINE_INTERLACED_PLANE_BIT_EXT,
  };
  profile.pNext = &decode_h264_profile;

  printf("%p %p\n", device, func);
}
