# fuurin
An experimental hardware-accelerated video decoder powered by Vulkan.

> An actual output from the app decoding H.264 video.
![output example](https://gist.githubusercontent.com/ogukei/539ffd202eeeddc1a503ba15c4773e3b/raw/5fc1501cb01db557a0e107b2316d92336396abde/out.png)

## Build
You will need the following environment.

- Ubuntu 18.04 LTS
- Vulkan Instance Version: 1.2.178
- NVIDIA Driver Version: 455.50.19

1. Install Vulkan-beta support graphics driver from https://developer.nvidia.com/vulkan-driver
1. Install Vulkan Loader following the instruction on https://github.com/KhronosGroup/Vulkan-Loader/blob/v1.2.178/BUILD.md
1. Clone the repository https://github.com/nvpro-samples/vk_video_samples
1. Copy [the bin directory in vk_video_samples](https://github.com/nvpro-samples/vk_video_samples/tree/95eeeb80879e04183923e2be3d0b93b3652ab868/vk_video_decoder/bin ) onto this repository.
Please make sure that you agree [the license agreement](https://github.com/nvpro-samples/vk_video_samples/blob/main/vk_video_decoder/bin/libs/nv_vkvideo_parser/LICENSE_NVIDIA_PARSER_LIBRARIES.txt) from Nvidia.
1. Run `apt install libavformat-dev libavcodec-dev libglm-dev`
1. Run `make`
1. Run `./main`
