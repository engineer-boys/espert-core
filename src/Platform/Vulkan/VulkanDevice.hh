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
    static bool s_is_exist;

    VkPhysicalDevice m_physical_device;
    VkDevice m_device;

   public:
    static std::unique_ptr<VulkanDevice> create();

    ~VulkanDevice() = default;

    VulkanDevice(const VulkanDevice&)            = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
    VkFormat
    find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    // Buffer Helper Functions
    void create_buffer(VkDeviceSize size,
                       VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags properties,
                       VkBuffer& buffer,
                       VkDeviceMemory& buffer_memory);
    void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
    void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);

    /*void create_image_with_info(const VkImageCreateInfo& image_info,
                                VkMemoryPropertyFlags properties,
                                VkImage& image,
                                VkDeviceMemory& image_memory);*/ // - swap chain

   private:
    VulkanDevice();
  };

} // namespace esp

#endif // RENDERER_ESP_DEVICE_HH
