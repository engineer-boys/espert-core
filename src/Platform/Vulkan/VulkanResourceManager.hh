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
    static void create_image(uint32_t width,
                             uint32_t height,
                             VkFormat format,
                             VkImageTiling tiling,
                             VkImageUsageFlags usage,
                             VkMemoryPropertyFlags properties,
                             VkImage& image,
                             VkDeviceMemory& imageMemory);

    static VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);
  };
} // namespace esp

#endif // VULKAN_RENDER_API_VULKAN_RESOURCE_MANAGER_HH
