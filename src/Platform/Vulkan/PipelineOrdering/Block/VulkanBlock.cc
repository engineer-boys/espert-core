#include "VulkanBlock.hh"
#include "Platform/Vulkan/Resources/VulkanTexture.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

/* VulkanRenderPlan is implemented under constraint this define is equal to 1 */
#define NUMBER_OF_BLOCK_BUFFER_IN_BLOCK 1

namespace esp
{
  void VulkanBlock::VulkanBlockBuffer::terminate()
  {
    vkDestroyImageView(VulkanDevice::get_logical_device(), m_image_view, nullptr);

    vkDestroyImage(VulkanDevice::get_logical_device(), m_image, nullptr);
    vkFreeMemory(VulkanDevice::get_logical_device(), m_image_memory, nullptr);
  }
} // namespace esp

namespace esp
{
  VulkanBlock::VulkanBlock(EspBlockFormat format,
                           EspSampleCountFlag sample_count_flag,
                           uint32_t width,
                           uint32_t height,
                           glm::vec3 clear_color) :
      EspBlock{ format, sample_count_flag, width, height, clear_color }
  {
    for (int i = 0; i < NUMBER_OF_BLOCK_BUFFER_IN_BLOCK; i++)
    {
      // Normal image: multisampled or non-multisampled
      {
        VulkanResourceManager::create_image(m_width,
                                            m_height,
                                            1,
                                            static_cast<VkSampleCountFlagBits>(m_sample_count_flag),
                                            static_cast<VkFormat>(m_format),
                                            VK_IMAGE_TILING_OPTIMAL,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                            1,
                                            {},
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            m_buffer.m_image,
                                            m_buffer.m_image_memory);

        m_buffer.m_image_view = VulkanResourceManager::create_image_view(m_buffer.m_image,
                                                                         static_cast<VkFormat>(m_format),
                                                                         VK_IMAGE_ASPECT_COLOR_BIT,
                                                                         1);
        m_buffer_exist        = true;
      }

      // If image is multisamples (sample_count_flag is not  ESP_SAMPLE_COUNT_1_BIT) then
      // the block will need resolve buffer.
      if (is_resolvable())
      {
        VulkanResourceManager::create_image(m_width,
                                            m_height,
                                            1,
                                            VK_SAMPLE_COUNT_1_BIT,
                                            static_cast<VkFormat>(m_format),
                                            VK_IMAGE_TILING_OPTIMAL,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                            1,
                                            {},
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            m_resolve_buffer.m_image,
                                            m_resolve_buffer.m_image_memory);

        m_resolve_buffer.m_image_view = VulkanResourceManager::create_image_view(m_resolve_buffer.m_image,
                                                                                 static_cast<VkFormat>(m_format),
                                                                                 VK_IMAGE_ASPECT_COLOR_BIT,
                                                                                 1);
        m_resolve_buffer_exist        = true;
      }
    }
  }

  std::shared_ptr<EspTexture> VulkanBlock::use_as_texture() const
  {
    return std::shared_ptr<EspTexture>{ VulkanTexture::create_from_block(this).release() };
  }

  VulkanBlock::~VulkanBlock()
  {
    if (m_buffer_exist) { m_buffer.terminate(); }

    if (m_resolve_buffer_exist) { m_resolve_buffer.terminate(); }
  }

  std::shared_ptr<EspTexture> VulkanBlock::extract_texture()
  {
    auto texture = std::shared_ptr<EspTexture>{ VulkanTexture::create_from_block(this, false).release() };

    if (is_resolvable()) { m_resolve_buffer_exist = false; }
    else { m_buffer_exist = false; }

    return texture;
  }
} // namespace esp
