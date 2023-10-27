#include "VulkanResourceManager.hh"
#include "VulkanCommandHandler.hh"
#include "VulkanDevice.hh"

namespace esp
{
  void VulkanResourceManager::allocate_buffer_on_device(VkDeviceSize size,
                                                        VkBufferUsageFlags usage,
                                                        VkMemoryPropertyFlags properties,
                                                        VkBuffer& buffer,
                                                        VkDeviceMemory& buffer_memory)
  {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size        = size;
    buffer_info.usage       = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanDevice::get_logical_device(), &buffer_info, nullptr, &buffer) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create buffer");
      throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(VulkanDevice::get_logical_device(), buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize  = mem_requirements.size;
    alloc_info.memoryTypeIndex = VulkanDevice::find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(VulkanDevice::get_logical_device(), &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate buffer memory");
      throw std::runtime_error("Failed to allocate buffer memory");
    }

    vkBindBufferMemory(VulkanDevice::get_logical_device(), buffer, buffer_memory, 0);
  }

  void VulkanResourceManager::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
  {
    VkCommandBuffer command_buffer = VulkanCommandHandler::begin_single_time_commands();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size      = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    VulkanCommandHandler::end_single_time_commands(command_buffer);
  }

  void VulkanResourceManager::copy_buffer_to_image(VkBuffer buffer,
                                                   VkImage image,
                                                   uint32_t width,
                                                   uint32_t height,
                                                   uint32_t layer_count)
  {
    VkCommandBuffer command_buffer = VulkanCommandHandler::begin_single_time_commands();

    VkBufferImageCopy region{};
    region.bufferOffset      = 0;
    region.bufferRowLength   = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = layer_count;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VulkanCommandHandler::end_single_time_commands(command_buffer);
  }

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