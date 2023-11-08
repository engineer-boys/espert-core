#include "VulkanResourceManager.hh"
#include "Resources/VulkanBuffer.hh"
#include "VulkanCommandHandler.hh"
#include "VulkanDevice.hh"

#if defined(__GNUC__) && !defined(NDEBUG) && defined(__OPTIMIZE__)
#warning "Undefing __OPTIMIZE__ because of fmt. This problem occurs after adding stb."
#undef __OPTIMIZE__
#endif

// stb
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
                                           uint32_t mip_levels,
                                           VkSampleCountFlagBits num_samples,
                                           VkFormat format,
                                           VkImageTiling tiling,
                                           VkImageUsageFlags usage,
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
    image_info.arrayLayers   = 1;
    image_info.format        = format;
    image_info.tiling        = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage         = usage;
    image_info.samples       = num_samples;
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

  void VulkanResourceManager::create_texture_image(const std::string& path,
                                                   uint32_t& texture_width,
                                                   uint32_t& texture_height,
                                                   uint32_t& texture_mip_levels,
                                                   VkImage& texture_image,
                                                   VkDeviceMemory& texture_image_memory)
  {
    int tex_width, tex_height, tex_channels;
    stbi_uc* pixels = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

    VkDeviceSize image_size = tex_width * tex_height * 4;

    ESP_ASSERT(pixels, "Failed to load texture image")

    texture_width      = static_cast<uint32_t>(tex_width);
    texture_height     = static_cast<uint32_t>(tex_height);
    texture_mip_levels = std::floor(std::log2(std::max(texture_width, texture_height))) + 1;

    VulkanBuffer staging_buffer{ image_size,
                                 1,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

    staging_buffer.map();
    staging_buffer.write_to_buffer(pixels);

    stbi_image_free(pixels);

    create_image(texture_width,
                 texture_height,
                 texture_mip_levels,
                 VK_SAMPLE_COUNT_1_BIT,
                 VK_FORMAT_R8G8B8A8_SRGB,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 texture_image,
                 texture_image_memory);

    transition_image_layout(texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            texture_mip_levels);

    copy_buffer_to_image(staging_buffer.get_buffer(), texture_image, texture_width, texture_height, 1);

    generate_mipmaps(texture_image, VK_FORMAT_R8G8B8A8_SRGB, texture_width, texture_height, texture_mip_levels);
  }

  void VulkanResourceManager::transition_image_layout(VkImage image,
                                                      VkFormat format,
                                                      VkImageLayout old_layout,
                                                      VkImageLayout new_layout,
                                                      uint32_t mip_levels)
  {
    VkCommandBuffer command_buffer = VulkanCommandHandler::begin_single_time_commands();

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = old_layout;
    barrier.newLayout                       = new_layout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

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

    VulkanCommandHandler::end_single_time_commands(command_buffer);
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

    VkCommandBuffer command_buffer = VulkanCommandHandler::begin_single_time_commands();

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

    VulkanCommandHandler::end_single_time_commands(command_buffer);
  }

  VulkanResourceManager::VulkanResourceManager()
  {
    ESP_ASSERT(VulkanResourceManager::s_instance == nullptr, "Vulkan resource manager already exists")

    s_instance = this;
  }
} // namespace esp