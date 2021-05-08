
#include <memory>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#include <vulkan/vulkan.h>

namespace vk {

class Instance {
 private:
  VkInstance instance_;
 public:
  static std::shared_ptr<Instance> Create();

  explicit Instance();
  Instance(const Instance &) = delete;
  ~Instance();

  void Initialize();
  VkInstance Handle() const { return instance_; }
};

} // namespace vk
