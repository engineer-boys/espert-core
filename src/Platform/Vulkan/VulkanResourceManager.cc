#include "VulkanResourceManager.hh"
#include "VulkanDevice.hh"

namespace esp
{
  void VulkanResourceManager::create_image(uint32_t width,
                                           uint32_t height,
                                           VkFormat format,
                                           VkImageTiling tiling,
                                           VkImageUsageFlags usage,
                                           VkMemoryPropertyFlags properties,
                                           VkImage& image,
                                           VkDeviceMemory& imageMemory)
  {
    VkImageCreateInfo image_info{};
    image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType     = VK_IMAGE_TYPE_2D;
    image_info.extent.width  = width;
    image_info.extent.height = height;
    image_info.extent.depth  = 1;
    image_info.mipLevels     = 1;
    image_info.arrayLayers   = 1;
    image_info.format        = format;
    image_info.tiling        = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage         = usage;
    image_info.samples       = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(VulkanDevice::get_logical_device(), &image_info, nullptr, &image) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create image");
      throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(VulkanDevice::get_logical_device(), image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize  = mem_requirements.size;
    alloc_info.memoryTypeIndex = VulkanDevice::find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(VulkanDevice::get_logical_device(), &alloc_info, nullptr, &imageMemory) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate image memory");
      throw std::runtime_error("Failed to allocate image memory");
    }

    vkBindImageMemory(VulkanDevice::get_logical_device(), image, imageMemory, 0);
  }

  VkImageView VulkanResourceManager::create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
  {
    VkImageViewCreateInfo view_info{};
    view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image                           = image;
    view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format                          = format;
    view_info.subresourceRange.aspectMask     = aspect_flags;
    view_info.subresourceRange.baseMipLevel   = 0;
    view_info.subresourceRange.levelCount     = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount     = 1;

    VkImageView image_view;
    if (vkCreateImageView(VulkanDevice::get_logical_device(), &view_info, nullptr, &image_view) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create image view");
      throw std::runtime_error("Failed to create image view");
    }

    return image_view;
  }
} // namespace esp