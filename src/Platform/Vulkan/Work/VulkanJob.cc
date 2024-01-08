#include "VulkanJob.hh"
#include "Platform/Vulkan/PipelineOrdering/VulkanCommandBuffer.hh"
#include "Platform/Vulkan/Resources/VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanJob* VulkanJob::s_instance = nullptr;

  std::unique_ptr<VulkanJob> VulkanJob::create()
  {
    ESP_ASSERT(VulkanJob::s_instance == nullptr, "VulkanJob already exists!");
    VulkanJob::s_instance = new VulkanJob();
    VulkanJob::s_instance->init();

    return std::unique_ptr<VulkanJob>{ VulkanJob::s_instance };
  }

  VulkanJob::VulkanJob() {}

  VulkanJob::~VulkanJob() {}

  void VulkanJob::init() {}

  void VulkanJob::terminate()
  {
    ESP_ASSERT(VulkanJob::s_instance != nullptr, "VulkanJob is deleted twice!");

    s_instance = nullptr;
  }

  void VulkanJob::done_all_jobs() { vkDeviceWaitIdle(VulkanDevice::get_logical_device()); }

  void VulkanJob::draw(uint32_t vertex_count)
  {
    vkCmdDraw(VulkanWorkOrchestrator::get_current_command_buffer(), vertex_count, 1, 0, 0);
  }

  void VulkanJob::draw(uint32_t vertex_count, uint32_t instance_count)
  {
    vkCmdDraw(VulkanWorkOrchestrator::get_current_command_buffer(), vertex_count, instance_count, 0, 0);
  }

  void VulkanJob::draw_indexed(uint32_t index_count)
  {
    vkCmdDrawIndexed(VulkanWorkOrchestrator::get_current_command_buffer(), index_count, 1, 0, 0, 0);
  }

  void VulkanJob::draw_indexed(uint32_t index_count, uint32_t instance_count)
  {
    vkCmdDrawIndexed(VulkanWorkOrchestrator::get_current_command_buffer(), index_count, instance_count, 0, 0, 0);
  }

  /*---------------------------------------------------------------------------*/

  void VulkanJob::draw(EspCommandBufferId* id, uint32_t vertex_count)
  {
    vkCmdDraw(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, vertex_count, 1, 0, 0);
  }

  void VulkanJob::draw(EspCommandBufferId* id, uint32_t vertex_count, uint32_t instance_count)
  {
    vkCmdDraw(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, vertex_count, instance_count, 0, 0);
  }

  void VulkanJob::draw_indexed(EspCommandBufferId* id, uint32_t index_count)
  {
    vkCmdDrawIndexed(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, index_count, 1, 0, 0, 0);
  }

  void VulkanJob::draw_indexed(EspCommandBufferId* id, uint32_t index_count, uint32_t instance_count)
  {
    vkCmdDrawIndexed(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer, index_count, instance_count, 0, 0, 0);
  }

  void VulkanJob::copy_image(EspCommandBufferId* id,
                             std::shared_ptr<EspTexture> src_texture,
                             EspImageLayout src_layout,
                             std::shared_ptr<EspTexture> dst_texture,
                             EspImageLayout dst_layout,
                             EspImageCopy region)
  {
    VkImageCopy copy_region = {};

    copy_region.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.srcSubresource.baseArrayLayer = region.src_subresource.m_base_array_layer;
    copy_region.srcSubresource.layerCount     = region.src_subresource.m_layer_count;
    copy_region.srcSubresource.mipLevel       = region.src_subresource.m_mip_level;
    copy_region.srcOffset                     = { 0, 0, 0 };

    copy_region.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.dstSubresource.baseArrayLayer = region.dst_subresource.m_base_array_layer;
    copy_region.dstSubresource.mipLevel       = region.dst_subresource.m_mip_level;
    copy_region.dstSubresource.layerCount     = region.dst_subresource.m_layer_count;
    copy_region.dstOffset                     = { 0, 0, 0 };

    copy_region.extent.width  = region.m_width;
    copy_region.extent.height = region.m_height;
    copy_region.extent.depth  = 1;

    vkCmdCopyImage(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer,
                   std::static_pointer_cast<VulkanTexture>(src_texture)->get_texture_image(),
                   static_cast<VkImageLayout>(src_layout),
                   std::static_pointer_cast<VulkanTexture>(dst_texture)->get_texture_image(),
                   static_cast<VkImageLayout>(dst_layout),
                   1,
                   &copy_region);
  }

  void VulkanJob::transform_image_layout(EspCommandBufferId* id,
                                         std::shared_ptr<EspTexture> texture,
                                         EspImageLayout old_layout,
                                         EspImageLayout new_layout,
                                         EspImageSubresourceRange image_subresource_range)
  {
    /*
      ref to the code which was used here:
      https://github.com/gfxstrand/SaschaWillems-Vulkan/blob/master/base/VulkanTools.cpp
    */

    auto old_image_layout = static_cast<VkImageLayout>(old_layout);
    auto new_image_layout = static_cast<VkImageLayout>(new_layout);
    auto vulkan_texture   = std::static_pointer_cast<VulkanTexture>(texture);

    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource_range.baseMipLevel            = image_subresource_range.m_base_mip_level;
    subresource_range.levelCount              = image_subresource_range.m_level_count;
    subresource_range.layerCount              = image_subresource_range.m_layer_count;
    subresource_range.baseArrayLayer          = image_subresource_range.m_base_array_layer;

    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.oldLayout           = old_image_layout;
    image_memory_barrier.newLayout           = new_image_layout;
    image_memory_barrier.image               = vulkan_texture->get_texture_image();
    image_memory_barrier.subresourceRange    = subresource_range;

    // Source layouts (old)
    // Source access mask controls actions that have to be finished on the old layout
    // before it will be transitioned to the new layout
    switch (old_image_layout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
      // Image layout is undefined (or does not matter)
      // Only valid as initial layout
      // No flags required, listed only for completeness
      image_memory_barrier.srcAccessMask = 0;
      break;

    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      // Image is preinitialized
      // Only valid as initial layout for linear images, preserves memory contents
      // Make sure host writes have been finished
      image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      // Image is a color attachment
      // Make sure any writes to the color buffer have been finished
      image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      // Image is a depth/stencil attachment
      // Make sure any writes to the depth/stencil buffer have been finished
      image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      // Image is a transfer source
      // Make sure any reads from the image have been finished
      image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      // Image is a transfer destination
      // Make sure any writes to the image have been finished
      image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      // Image is read by a shader
      // Make sure any shader reads from the image have been finished
      image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
      break;
    default:
      // Other source layouts aren't handled (yet)
      break;
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (new_image_layout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      // Image will be used as a transfer destination
      // Make sure any writes to the image have been finished
      image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      // Image will be used as a transfer source
      // Make sure any reads from the image have been finished
      image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      // Image will be used as a color attachment
      // Make sure any writes to the color buffer have been finished
      image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      // Image layout will be used as a depth/stencil attachment
      // Make sure any writes to depth/stencil buffer have been finished
      image_memory_barrier.dstAccessMask =
          image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      // Image will be read in a shader (sampler, input attachment)
      // Make sure any writes to the image have been finished
      if (image_memory_barrier.srcAccessMask == 0)
      {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      }
      image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      break;
    default:
      // Other source layouts aren't handled (yet)
      break;
    }

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(static_cast<VulkanCommandBufferId*>(id)->m_command_buffer,
                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &image_memory_barrier);
  }
} // namespace esp
