
#include "vk/video_h264_picture_parameters.h"

namespace vk {

H264PictureParameters::H264PictureParameters() {
}

void H264PictureParameters::ConfigureSequenceParameterSet(
    const StdVideoH264SequenceParameterSet* sps,
    const StdVideoH264SequenceParameterSetVui *sps_vui,
    const StdVideoH264ScalingLists *sps_scaling_lists) {
  sps_ = *sps;
  if (sps_vui != nullptr) {
    sps_vui_ = *sps_vui;
    sps_->pSequenceParameterSetVui = &sps_vui_.value();
  }
  if (sps_scaling_lists != nullptr) {
    sps_scaling_lists_ = *sps_scaling_lists;
    sps_->pScalingLists = &sps_scaling_lists_.value();
  }
}

void H264PictureParameters::ConfigurePictureParameterSet(
    const StdVideoH264PictureParameterSet* pps,
    const StdVideoH264ScalingLists* pps_scaling_lists) {
  pps_ = *pps;
  if (pps_scaling_lists != nullptr) {
    pps_scaling_lists_ = *pps_scaling_lists;
    pps_->pScalingLists = &pps_scaling_lists_.value();
  }
}

H264PictureParameters::~H264PictureParameters() {
}

}  // namespace vk
