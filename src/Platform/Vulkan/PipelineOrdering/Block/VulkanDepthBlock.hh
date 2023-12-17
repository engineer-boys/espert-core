#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH

// libs
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/Block/EspDepthBlock.hh"

namespace esp
{
  class VulkanDepthBlock : public EspDepthBlock
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    // // If the image is from the swap chain this field is FLASE. Otherwise it is true.
    // bool m_delete_image;

    // VkImage m_image;
    // VkImageView m_image_view;
    // VkDeviceMemory m_image_memory;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanDepthBlock(uint32_t width, uint32_t height, glm::vec3 clear_color);
    // VulkanBlock(VkImage image, VkFormat format, uint32_t width, uint32_t height);
    virtual ~VulkanDepthBlock();

    VulkanDepthBlock(const VulkanDepthBlock&)            = delete;
    VulkanDepthBlock& operator=(const VulkanDepthBlock&) = delete;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH */