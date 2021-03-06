
#include "vk/video_frame.h"

#include "vk/device.h"
#include "vk/video_image.h"
#include "vk/video_image_memory.h"
#include "vk/image_view.h"

namespace vk {

std::shared_ptr<VideoSessionFrame> VideoSessionFrame::Create(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format) {
  auto video_frame = std::make_shared<VideoSessionFrame>(device, queue, video_profile, width, height, format);
  video_frame->Initialize();
  return video_frame;
}

VideoSessionFrame::VideoSessionFrame(
    const std::shared_ptr<vk::Device>& device,
    const std::shared_ptr<vk::Queue>& queue,
    const std::shared_ptr<vk::VideoProfile>& video_profile,
    uint32_t width,
    uint32_t height,
    VkFormat format)
    : device_(device),
      queue_(queue),
      video_profile_(video_profile),
      width_(width),
      height_(height),
      format_(format) {
}

void VideoSessionFrame::Initialize() {
  image_ = VideoImage::Create(
    device_, queue_, video_profile_, width_, height_, format_);
  image_memory_ = VideoImageMemory::Create(image_);
  image_view_ = ImageView::Create(device_, image_->Handle(), format_, VK_IMAGE_ASPECT_COLOR_BIT);
}

VideoSessionFrame::~VideoSessionFrame() {
}

}  // namespace vk
