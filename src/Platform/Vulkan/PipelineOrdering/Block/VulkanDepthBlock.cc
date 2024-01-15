#include "VulkanDepthBlock.hh"

#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  void VulkanDepthBlock::VulkanDepthBlockBuffer::terminate()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_image_view, nullptr);

    vkDestroyImage(VulkanDevice::get_logical_device(), m_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_image_memory, nullptr);
  }
} // namespace esp

namespace esp
{
  VulkanDepthBlock::VulkanDepthBlock(EspDepthBlockFormat format,
                                     EspSampleCountFlag sample_count_flag,
                                     EspImageUsageFlag image_usage_flag,
                                     uint32_t width,
                                     uint32_t height) :
      EspDepthBlock(format, sample_count_flag, image_usage_flag, width, height)
  {
    VulkanResourceManager::create_image(m_width,
                                        m_height,
                                        1,
                                        static_cast<VkSampleCountFlagBits>(m_sample_count_flag),
                                        static_cast<VkFormat>(m_format),
                                        VK_IMAGE_TILING_OPTIMAL,
                                        static_cast<VkImageUsageFlags>(m_image_usage_flag),
                                        1,
                                        {},
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        m_buffer.m_image,
                                        m_buffer.m_image_memory);

    m_buffer.m_image_view = VulkanResourceManager::create_image_view(m_buffer.m_image,
                                                                     static_cast<VkFormat>(m_format),
                                                                     VK_IMAGE_ASPECT_DEPTH_BIT,
                                                                     1);

    // If image is multisampled (sample_count_flag is not  ESP_SAMPLE_COUNT_1_BIT) then
    // the block will need resolve buffer.
    if (is_resolvable())
    {
      VulkanResourceManager::create_image(m_width,
                                          m_height,
                                          1,
                                          VK_SAMPLE_COUNT_1_BIT,
                                          static_cast<VkFormat>(m_format),
                                          VK_IMAGE_TILING_OPTIMAL,
                                          static_cast<VkImageUsageFlags>(m_image_usage_flag),
                                          1,
                                          {},
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          m_resolve_buffer.m_image,
                                          m_resolve_buffer.m_image_memory);

      m_resolve_buffer.m_image_view = VulkanResourceManager::create_image_view(m_resolve_buffer.m_image,
                                                                               static_cast<VkFormat>(m_format),
                                                                               VK_IMAGE_ASPECT_DEPTH_BIT,
                                                                               1);
      m_resolve_buffer_exist        = true;
    }

    m_need_layout_transition = true;
  }

  VulkanDepthBlock::~VulkanDepthBlock()
  {
    m_buffer.terminate();

    if (m_resolve_buffer_exist) { m_resolve_buffer.terminate(); }
  }
} // namespace esp
