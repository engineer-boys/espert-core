#ifndef VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH
#define VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH

#include "esppch.hh"

// libs
#include "volk.h"

namespace esp
{
  class VulkanResourceManager
  {
   public:
    // ---------------------------------------- Buffer Helper Functions ----------------------------------------
    static void allocate_buffer_on_device(VkDeviceSize size,
                                          VkBufferUsageFlags usage,
                                          VkMemoryPropertyFlags properties,
                                          VkBuffer& buffer,
                                          VkDeviceMemory& buffer_memory);

    static void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

    static void
    copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);
    // ---------------------------------------------------------------------------------------------------------

    // ---------------------------------------- Image Helper Functions ----------------------------------------
    static void create_image(uint32_t width,
                             uint32_t height,
                             VkFormat format,
                             VkImageTiling tiling,
                             VkImageUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkImage& image,
                             VkDeviceMemory& imageMemory);

    static VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
    // ---------------------------------------------------------------------------------------------------------
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH
