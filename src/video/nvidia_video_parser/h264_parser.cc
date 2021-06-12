
#include "video/nvidia_video_parser/h264_parser.h"

#include <iostream>

#include "video/bitstream_segment.h"
#include "video/h264_picture_parameters.h"

namespace video {
namespace nvidia_video_parser {

class PictureBuffer : public vkPicBuffBase {
 private:
 public:
  PictureBuffer() {}
  virtual ~PictureBuffer() {}
};

H264Parser::H264Parser() : parser_(nullptr) {
  bool is_annex_b = true;
  bool ok = CreateVulkanVideoDecodeParser(&parser_, VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_EXT, is_annex_b);
  assert(ok);
  picture_parameters_ = std::make_unique<H264PictureParameters>();
}

void H264Parser::Initialize() {
  static const VkExtensionProperties std_extension_name = {
    VK_STD_VULKAN_VIDEO_CODEC_H264_EXTENSION_NAME,
    VK_STD_VULKAN_VIDEO_CODEC_H264_SPEC_VERSION
  };
  // configure parameters
  VkParserInitDecodeParameters parameters = {};
  parameters.interfaceVersion = NV_VULKAN_VIDEO_PARSER_API_VERSION;
  parameters.pClient = reinterpret_cast<VkParserVideoDecodeClient*>(this);
  // use default
  parameters.lReferenceClockRate = 0;
  // do not decode if error
  parameters.lErrorThreshold = 0;
  parameters.bOutOfBandPictureParameters = true;
  VkResult ok = parser_->Initialize(&parameters);
  assert(ok == VK_SUCCESS);
}

void H264Parser::Parse(const BitstreamSegment& input) {
  VkParserBitstreamPacket bitstream_packet = {};
  bitstream_packet.pByteStream = input.data;
  bitstream_packet.nDataLength = input.size;
  bitstream_packet.bEOS = (input.size == 0);
  bitstream_packet.bEOP = false;
  bitstream_packet.bPTSValid = true;
  bitstream_packet.llPTS = input.presentation_timestamp;
  bool ok = parser_->ParseByteStream(&bitstream_packet);
  assert(ok);
}

H264Parser::~H264Parser() {
  if (parser_ != nullptr) {
    parser_->Deinitialize();
    parser_->Release();
    parser_ = nullptr;
  }
}

int32_t H264Parser::BeginSequence(const VkParserSequenceInfo* info) {
  int32_t dpb_count = 4;
  std::cout << "H264Parser::BeginSequence" << std::endl;
  std::cout << "nCodedWidth " << info->nCodedWidth  << std::endl;
  std::cout << "nCodedHeight " << info->nCodedHeight  << std::endl;

  return dpb_count;
}

bool H264Parser::AllocPictureBuffer(VkPicIf** buffer) {
  // FIXME(ogukei): fix memory leaks
  auto* instance = new PictureBuffer();
  instance->AddRef();
  *buffer = instance;
  return true;
}

bool H264Parser::DecodePicture(VkParserPictureData* data) {
  std::cout << "H264Parser::DecodePicture" << std::endl;
  return false;
}

bool H264Parser::UpdatePictureParameters(
    VkPictureParameters* parameters,
    VkSharedBaseObj<VkParserVideoRefCountBase>& object,  // NOLINT
    uint64_t update_sequence_count) {
  std::cout << "H264Parser::UpdatePictureParameters" << std::endl;
  switch (parameters->updateType) {
  case VK_PICTURE_PARAMETERS_UPDATE_H264_SPS:
    picture_parameters_->ConfigureSequenceParameterSet(
      parameters->pH264Sps,
      parameters->pH264Sps->pSequenceParameterSetVui,
      parameters->pH264Sps->pScalingLists);
    break;
  case VK_PICTURE_PARAMETERS_UPDATE_H264_PPS:
    picture_parameters_->ConfigurePictureParameterSet(
      parameters->pH264Pps,
      parameters->pH264Pps->pScalingLists);
    break;
  default:
    break;
  }
  return false;
}

bool H264Parser::DisplayPicture(VkPicIf* buffer, int64_t pts) {
  return false;
}

void H264Parser::UnhandledNALU(const uint8_t* data, int32_t cbdata) {
}

}  // namespace nvidia_video_parser
}  // namespace video
