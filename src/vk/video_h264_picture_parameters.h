

#pragma once

extern "C" {
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <memory>
#include <optional>

namespace vk {

class H264PictureParameters {
 private:
  std::optional<StdVideoH264SequenceParameterSet> sps_;
  std::optional<StdVideoH264SequenceParameterSetVui> sps_vui_;
  std::optional<StdVideoH264ScalingLists> sps_scaling_lists_;

  std::optional<StdVideoH264PictureParameterSet> pps_;
  std::optional<StdVideoH264ScalingLists> pps_scaling_lists_;

 public:
  H264PictureParameters();
  H264PictureParameters(const H264PictureParameters&) = delete;
  ~H264PictureParameters();

  void ConfigureSequenceParameterSet(
    const StdVideoH264SequenceParameterSet* sps,
    const StdVideoH264SequenceParameterSetVui* sps_vui,
    const StdVideoH264ScalingLists* sps_scaling_lists);

  void ConfigurePictureParameterSet(
    const StdVideoH264PictureParameterSet* pps,
    const StdVideoH264ScalingLists* pps_scaling_lists);

  const std::optional<StdVideoH264SequenceParameterSet>& SequenceParameterSet() const { return sps_; }
  const std::optional<StdVideoH264PictureParameterSet>& PictureParameterSet() const { return pps_; }
};

}  // namespace vk
