#include "VulkanBlock.hh"
#include "Platform/Vulkan/VulkanDevice.hh"
#include "Platform/Vulkan/VulkanResourceManager.hh"

/* VulkanProductPlan is implemented under constraint this define is equal to 1 */
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
  VulkanBlock::VulkanBlock(EspBlockFormat format, uint32_t width, uint32_t height, glm::vec3 clear_color) :
      EspBlock{ format, width, height, clear_color }
  {
    for (int i = 0; i < NUMBER_OF_BLOCK_BUFFER_IN_BLOCK; i++)
    {
      VulkanBlockBuffer buffer = {};

      VulkanResourceManager::create_image(m_width,
                                          m_height,
                                          1,
                                          VK_SAMPLE_COUNT_1_BIT,
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
  }

  VulkanBlock::~VulkanBlock()
  {
    for (auto buffer : m_buffers)
    {
      buffer.terminate();
    }
    m_buffers.clear();
  }
} // namespace esp
