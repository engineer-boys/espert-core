#include "VulkanBlock.hh"
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
        VulkanBlockBuffer buffer = {};

        VulkanResourceManager::create_image(m_width,
                                            m_height,
                                            1,
                                            static_cast<VkSampleCountFlagBits>(m_sample_count_flag),
                                            static_cast<VkFormat>(m_format),
                                            VK_IMAGE_TILING_OPTIMAL,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            buffer.m_image,
                                            buffer.m_image_memory);

        buffer.m_image_view = VulkanResourceManager::create_image_view(buffer.m_image,
                                                                       static_cast<VkFormat>(m_format),
                                                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                                                       1);

        m_buffers.push_back(buffer);
      }

      // If image is multisamples (sample_count_flag is not  ESP_SAMPLE_COUNT_1_BIT) then
      // the block will need resolve buffer.
      if (is_resolvable())
      {
        VulkanBlockBuffer resolve_buffer = {};
        VulkanResourceManager::create_image(m_width,
                                            m_height,
                                            1,
                                            VK_SAMPLE_COUNT_1_BIT,
                                            static_cast<VkFormat>(m_format),
                                            VK_IMAGE_TILING_OPTIMAL,
                                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            resolve_buffer.m_image,
                                            resolve_buffer.m_image_memory);

        resolve_buffer.m_image_view = VulkanResourceManager::create_image_view(resolve_buffer.m_image,
                                                                               static_cast<VkFormat>(m_format),
                                                                               VK_IMAGE_ASPECT_COLOR_BIT,
                                                                               1);

        m_resolve_buffers.push_back(resolve_buffer);
      }
    }
  }

  VulkanBlock::~VulkanBlock()
  {
    for (auto buffer : m_buffers)
    {
      buffer.terminate();
    }
    m_buffers.clear();

    for (auto resolve_buffer : m_resolve_buffers)
    {
      resolve_buffer.terminate();
    }
    m_resolve_buffers.clear();
  }
} // namespace esp
