#include "VulkanResourceManager.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"
#include "Resources/VulkanBuffer.hh"
#include "VulkanDevice.hh"

namespace esp
{
  VulkanResourceManager* VulkanResourceManager::s_instance = nullptr;

  std::unique_ptr<VulkanResourceManager> VulkanResourceManager::create()
  {
    return std::unique_ptr<VulkanResourceManager>(new VulkanResourceManager());
  }

  VulkanResourceManager::~VulkanResourceManager() { s_instance = nullptr; }

  void VulkanResourceManager::terminate() {}

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
    VkCommandBuffer command_buffer = VulkanWorkOrchestrator::begin_single_time_commands();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size      = size;
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

    VulkanWorkOrchestrator::end_single_time_commands(command_buffer);
  }

  void VulkanResourceManager::copy_buffer_to_image(VkBuffer buffer,
                                                   VkImage image,
                                                   uint32_t width,
                                                   uint32_t height,
                                                   uint32_t layer_count,
                                                   uint32_t region_count,
                                                   uint32_t layer_size)
  {
    VkCommandBuffer command_buffer = VulkanWorkOrchestrator::begin_single_time_commands();

    std::vector<VkBufferImageCopy> buffer_copy_regions;

    for (int i = 0; i < region_count; ++i)
    {
      VkBufferImageCopy region{};
      region.bufferOffset      = i * layer_size;
      region.bufferRowLength   = 0;
      region.bufferImageHeight = 0;

      region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
      region.imageSubresource.mipLevel       = 0;
      region.imageSubresource.baseArrayLayer = i;
      region.imageSubresource.layerCount     = 1;

      region.imageOffset = { 0, 0, 0 };
      region.imageExtent = { width, height, 1 };

      buffer_copy_regions.push_back(region);
    }

    vkCmdCopyBufferToImage(command_buffer,
                           buffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           buffer_copy_regions.size(),
                           buffer_copy_regions.data());

    VulkanWorkOrchestrator::end_single_time_commands(command_buffer);
  }

  void VulkanResourceManager::create_image(uint32_t width,
                                           uint32_t height,
                                           uint32_t mip_levels,
                                           VkSampleCountFlagBits num_samples,
                                           VkFormat format,
                                           VkImageTiling tiling,
                                           VkImageUsageFlags usage,
                                           uint32_t layers,
                                           VkImageCreateFlags flags,
                                           VkMemoryPropertyFlags properties,
                                           VkImage& image,
                                           VkDeviceMemory& image_memory)
  {
    VkImageCreateInfo image_info{};
    image_info.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType     = VK_IMAGE_TYPE_2D;
    image_info.extent.width  = width;
    image_info.extent.height = height;
    image_info.extent.depth  = 1;
    image_info.mipLevels     = mip_levels;
    image_info.arrayLayers   = layers;
    image_info.format        = format;
    image_info.tiling        = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage         = usage;
    image_info.samples       = num_samples;
    image_info.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags         = flags;

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

    if (vkAllocateMemory(VulkanDevice::get_logical_device(), &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to allocate image memory");
      throw std::runtime_error("Failed to allocate image memory");
    }

    vkBindImageMemory(VulkanDevice::get_logical_device(), image, image_memory, 0);
  }

  VkImageView VulkanResourceManager::create_image_view(VkImage image,
                                                       VkFormat format,
                                                       VkImageAspectFlags aspect_flags,
                                                       uint32_t mip_levels)
  {
    VkImageViewCreateInfo view_info{};
    view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image                           = image;
    view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format                          = format;
    view_info.subresourceRange.aspectMask     = aspect_flags;
    view_info.subresourceRange.baseMipLevel   = 0;
    view_info.subresourceRange.levelCount     = mip_levels;
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

  VkImageView VulkanResourceManager::create_cubemap_image_view(VkImage image,
                                                               VkFormat format,
                                                               VkImageAspectFlags aspect_flags,
                                                               uint32_t mip_levels)
  {
    VkImageViewCreateInfo view_info{};
    view_info.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image            = image;
    view_info.viewType         = VK_IMAGE_VIEW_TYPE_CUBE;
    view_info.format           = format;
    view_info.subresourceRange = { aspect_flags, 0, mip_levels, 0, 6 };

    VkImageView image_view;
    if (vkCreateImageView(VulkanDevice::get_logical_device(), &view_info, nullptr, &image_view) != VK_SUCCESS)
    {
      ESP_CORE_ERROR("Failed to create image view");
      throw std::runtime_error("Failed to create image view");
    }

    return image_view;
  }

  void VulkanResourceManager::create_texture_image(uint32_t width,
                                                   uint32_t height,
                                                   const void* pixels,
                                                   uint32_t mip_levels,
                                                   VkImage& texture_image,
                                                   VkDeviceMemory& texture_image_memory)
  {
    VkDeviceSize image_size = width * height * 4;

    VulkanBuffer staging_buffer{ image_size,
                                 1,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

    staging_buffer.map();
    staging_buffer.write_to_buffer(pixels);

    create_image(width,
                 height,
                 mip_levels,
                 VK_SAMPLE_COUNT_1_BIT,
                 VK_FORMAT_R8G8B8A8_SRGB,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 1,
                 {},
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 texture_image,
                 texture_image_memory);

    VkImageSubresourceRange subresource_range;
    subresource_range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel   = 0;
    subresource_range.levelCount     = mip_levels;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount     = 1;

    transition_image_layout(texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            mip_levels,
                            subresource_range);

    copy_buffer_to_image(staging_buffer.get_buffer(), texture_image, width, height, 1);

    generate_mipmaps(texture_image, VK_FORMAT_R8G8B8A8_SRGB, width, height, mip_levels);
  }

  void VulkanResourceManager::create_cubemap_image(uint32_t width,
                                                   uint32_t height,
                                                   const void* pixels[6],
                                                   uint32_t mip_levels,
                                                   VkImage& cubemap_image,
                                                   VkDeviceMemory& cubemap_image_memory)
  {
    // NOW: mip_levels has to be always 1
    VkDeviceSize layer_size = width * height * 4;
    VkDeviceSize image_size = layer_size * 6;

    VulkanBuffer staging_buffer{ image_size,
                                 1,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

    staging_buffer.map();

    for (int i = 0; i < 6; ++i)
    {
      staging_buffer.write_to_buffer(pixels[i], layer_size, i * layer_size);
    }

    create_image(width,
                 height,
                 mip_levels,
                 VK_SAMPLE_COUNT_1_BIT,
                 VK_FORMAT_R8G8B8A8_SRGB,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 6,
                 VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 cubemap_image,
                 cubemap_image_memory);

    VkImageSubresourceRange subresource_range;
    subresource_range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel   = 0;
    subresource_range.levelCount     = mip_levels;
    subresource_range.baseArrayLayer = 0;
    subresource_range.layerCount     = 6;

    transition_image_layout(cubemap_image,
                            VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            mip_levels,
                            subresource_range);

    copy_buffer_to_image(staging_buffer.get_buffer(), cubemap_image, width, height, 1, 6, layer_size);

    transition_image_layout(cubemap_image,
                            VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            mip_levels,
                            subresource_range);
  }

  void VulkanResourceManager::transition_image_layout(VkImage image,
                                                      VkFormat format,
                                                      VkImageLayout old_layout,
                                                      VkImageLayout new_layout,
                                                      uint32_t mip_levels,
                                                      VkImageSubresourceRange subresource_range)
  {
    VkCommandBuffer command_buffer = VulkanWorkOrchestrator::begin_single_time_commands();

    VkImageMemoryBarrier barrier{};
    barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout           = old_layout;
    barrier.newLayout           = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = image;
    barrier.subresourceRange    = subresource_range;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags destination_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      source_stage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
      ESP_CORE_ERROR("Unsupported layout transition");
      throw std::invalid_argument("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    VulkanWorkOrchestrator::end_single_time_commands(command_buffer);
  }

  void VulkanResourceManager::generate_mipmaps(VkImage image,
                                               VkFormat format,
                                               uint32_t texture_width,
                                               uint32_t texture_height,
                                               uint32_t mip_levels)
  {
    // Check if image format supports linear blitting
    if (!(VulkanDevice::get_format_properties(format).optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
      ESP_CORE_ERROR("Texture image format does not support linear blitting");
      throw std::runtime_error("Texture image format does not support linear blitting");
    }

    VkCommandBuffer command_buffer = VulkanWorkOrchestrator::begin_single_time_commands();

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image                           = image;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    int32_t mip_width  = texture_width;
    int32_t mip_height = texture_height;

    for (uint32_t i = 1; i < mip_levels; i++)
    {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(command_buffer,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &barrier);

      VkImageBlit blit{};
      blit.srcOffsets[0]                 = { 0, 0, 0 };
      blit.srcOffsets[1]                 = { mip_width, mip_height, 1 };
      blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.srcSubresource.mipLevel       = i - 1;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount     = 1;
      blit.dstOffsets[0]                 = { 0, 0, 0 };
      blit.dstOffsets[1]             = { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 };
      blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.dstSubresource.mipLevel   = i;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount     = 1;

      vkCmdBlitImage(command_buffer,
                     image,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                     image,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                     1,
                     &blit,
                     VK_FILTER_LINEAR);

      barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      vkCmdPipelineBarrier(command_buffer,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &barrier);

      if (mip_width > 1) mip_width /= 2;
      if (mip_height > 1) mip_height /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    VulkanWorkOrchestrator::end_single_time_commands(command_buffer);
  }

  VulkanResourceManager::VulkanResourceManager()
  {
    ESP_ASSERT(VulkanResourceManager::s_instance == nullptr, "Vulkan resource manager already exists")

    s_instance = this;
  }
} // namespace esp