
#include <memory>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

namespace vk {

class Instance;

class PhysicalDevice {
 private:
  std::shared_ptr<vk::Instance> instance_;
  VkPhysicalDevice physical_device_;
 public:
  static std::shared_ptr<PhysicalDevice> Create(
    std::shared_ptr<Instance>& instance
  );

  explicit PhysicalDevice(std::shared_ptr<vk::Instance>& instance);
  PhysicalDevice(const PhysicalDevice&) = delete;
  ~PhysicalDevice();

  void Initialize();
  VkPhysicalDevice Handle() const { return physical_device_; }
  const std::shared_ptr<vk::Instance>& Instance() const { return instance_; }
};

} // namespace vk
