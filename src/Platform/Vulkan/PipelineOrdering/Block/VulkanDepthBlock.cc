#include "VulkanDepthBlock.hh"

#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

namespace esp
{
  VulkanDepthBlock::VulkanDepthBlock(EspDepthBlockFormat format,
                                     EspSampleCountFlag sample_count_flag,
                                     uint32_t width,
                                     uint32_t height) :
      EspDepthBlock(format, sample_count_flag, width, height)
  {
    VulkanResourceManager::create_image(m_width,
                                        m_height,
                                        1,
                                        static_cast<VkSampleCountFlagBits>(m_sample_count_flag),
                                        static_cast<VkFormat>(m_format),
                                        VK_IMAGE_TILING_OPTIMAL,
                                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        1,
                                        {},
                                        m_image,
                                        m_image_memory);

    m_image_view = VulkanResourceManager::create_image_view(m_image,
                                                            static_cast<VkFormat>(m_format),
                                                            VK_IMAGE_ASPECT_DEPTH_BIT,
                                                            1);

    m_need_layout_transition = true;
  }

  VulkanDepthBlock::~VulkanDepthBlock()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_image_view, nullptr);

    vkDestroyImage(VulkanDevice::get_logical_device(), m_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_image_memory, nullptr);
  }
} // namespace esp
