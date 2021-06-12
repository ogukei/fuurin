
#pragma once

#include <memory>
#include <string>

#include "nvidia_video_parser/nvidia_video_parser.h"

namespace vk {
class H264PictureParameters;
}  // namespace vk

namespace video {

class BitstreamSegment;

namespace nvidia_video_parser {

class H264Parser : public VkParserVideoDecodeClient {
 private:
  VulkanVideoDecodeParser* parser_;
  std::shared_ptr<vk::H264PictureParameters> picture_parameters_;
  bool is_sequence_ready_;

 public:
  H264Parser();
  virtual ~H264Parser();

  void Initialize();
  void Parse(const BitstreamSegment& bitstream_segment);

  bool IsSequenceReady() const { return is_sequence_ready_; }
  const std::shared_ptr<vk::H264PictureParameters>& PictureParameters() const { return picture_parameters_; }

  // VkParserVideoDecodeClient
  // @see https://github.com/nvpro-samples/vk_video_samples/blob/95eeeb80879e04183923e2be3d0b93b3652ab868/vk_video_decoder/include/vkvideo_parser/VulkanVideoParserIf.h#L689
  virtual int32_t BeginSequence(const VkParserSequenceInfo* info);
  virtual bool AllocPictureBuffer(VkPicIf** buffer);
  virtual bool DecodePicture(VkParserPictureData* data);
  virtual bool UpdatePictureParameters(
    VkPictureParameters* parameters,
    VkSharedBaseObj<VkParserVideoRefCountBase>& object,  // NOLINT
    uint64_t update_sequence_count);
  virtual bool DisplayPicture(VkPicIf* buffer, int64_t pts);
  virtual void UnhandledNALU(const uint8_t* data, int32_t cbdata);
};

}  // namespace nvidia_video_parser
}  // namespace video
