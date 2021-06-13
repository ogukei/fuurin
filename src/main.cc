
extern "C" {
// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>
}

#include <iostream>

#include "app/render.h"

int main(int argc, const char** argv) {
  if (argc != 2) {
    std::cout << "Usage: ./main BigBuckBunny.mp4" << std::endl;
    std::cout << "* http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4" << std::endl;
    return 0;
  }
  std::string filename = argv[1];
  auto* app = new Render();
  app->Run(filename);
  return 0;
}
