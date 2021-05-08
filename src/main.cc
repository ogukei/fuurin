
extern "C" {
#include <stdio.h>
}

// GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

// Others
#include "render.h"

int main(void) {
  if (!glfwInit()) {
    return 0;
  }
  if (!glfwVulkanSupported()) {
    return 0;
  }
  {
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    glfwMakeContextCurrent(window);
    new Render();
    while (!glfwWindowShouldClose(window)) {
      glfwWaitEvents();
    }
    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
  }
  glfwTerminate();
  return 0;
}
