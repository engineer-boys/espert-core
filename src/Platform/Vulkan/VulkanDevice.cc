#include "VulkanCommandHandler.hh"
#include "VulkanContext.hh"
#include "VulkanDevice.hh"

namespace esp
{
  bool VulkanDevice::s_is_exist = false;

  std::unique_ptr<VulkanDevice> VulkanDevice::create()
  {
    return std::unique_ptr<VulkanDevice>(new VulkanDevice());
  }

  uint32_t VulkanDevice::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
    {
      if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        return i;
      }
    }

    ESP_CORE_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("Failed to find suitable memory type");
  }

  VkFormat VulkanDevice::find_supported_format(const std::vector<VkFormat>& candidates,
                                                      VkImageTiling tiling,
                                                      VkFormatFeatureFlags features)
  {
    for (VkFormat format : candidates)
    {
      VkFormatProperties props;
      vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

      if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) { return format; }
      else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
      {
        return format;
      }
    }

    ESP_CORE_ERROR("Failed to find supported format");
    throw std::runtime_error("Failed to find supported format");
  }

  void VulkanDevice::create_buffer(VkDeviceSize size,
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

    if (vkCreateBuffer(m_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create buffer");
      throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(m_device, buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize  = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate buffer memory");
      throw std::runtime_error("Failed to allocate buffer memory");
    }

    vkBindBufferMemory(m_device, buffer, buffer_memory, 0);
  }

  void VulkanDevice::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
  {
    VkCommandBuffer command_buffer = VulkanCommandHandler::begin_single_time_commands();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size      = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    VulkanCommandHandler::end_single_time_commands(command_buffer);
  }

  void VulkanDevice::copy_buffer_to_image(VkBuffer buffer,
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

  void VulkanDevice::create_image_with_info(const VkImageCreateInfo& image_info,
                                            VkMemoryPropertyFlags properties,
                                            VkImage& image,
                                            VkDeviceMemory& image_memory)
  {
    if (vkCreateImage(m_device, &image_info, nullptr, &image) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create image");
      throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(m_device, image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize  = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate image memory");
      throw std::runtime_error("Failed to allocate image memory");
    }

    if (vkBindImageMemory(m_device, image, image_memory, 0) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to bind image memory");
      throw std::runtime_error("Failed to bind image memory");
    }
  }

  VulkanDevice::VulkanDevice()
  {
    ESP_ASSERT(!VulkanDevice::s_is_exist, "Vulkan device already exists")

    auto& context_data = VulkanContext::get_context_data();

    s_is_exist = true;

    m_physical_device = context_data.m_physical_device;
    m_device          = context_data.m_device;
  }
} // namespace esp