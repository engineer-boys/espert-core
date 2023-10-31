#ifndef RENDERER_ESP_DEVICE_HH
#define RENDERER_ESP_DEVICE_HH

// libs
#include "volk.h"

// std
#include <string>
#include <vector>

namespace esp
{
  class VulkanDevice
  {
   private:
    static VulkanDevice* s_instance;

    VkPhysicalDevice m_physical_device;
    VkDevice m_device;

   public:
    static std::unique_ptr<VulkanDevice> create(VkPhysicalDevice physical_device, VkDevice device);

    VulkanDevice(const VulkanDevice&)            = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    ~VulkanDevice();

    static inline const VkDevice& get_logical_device() { return s_instance->m_device; }

    static void complete_queues();

    void terminate();

    // -------------------------------------- Swap Chain Helper Functions --------------------------------------
    static VkFormat
    find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    static uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
    // ---------------------------------------------------------------------------------------------------------

   private:
    VulkanDevice(VkPhysicalDevice physical_device, VkDevice device);
  };

} // namespace esp

#endif // RENDERER_ESP_DEVICE_HH
