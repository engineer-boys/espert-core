#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH

// libs
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/Block/EspBlock.hh"

namespace esp
{
  class VulkanBlock : public EspBlock
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    struct VulkanBlockBuffer
    {
      VkImage m_image;
      VkImageView m_image_view;
      VkDeviceMemory m_image_memory;

      void terminate();
    };

    std::vector<VulkanBlockBuffer> m_buffers;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanBlock(EspBlockFormat format, uint32_t width, uint32_t height, glm::vec3 clear_color);
    virtual ~VulkanBlock();

    VulkanBlock(const VulkanBlock&)            = delete;
    VulkanBlock& operator=(const VulkanBlock&) = delete;

    inline VkImage get_image() const { return m_buffers[0].m_image; }
    inline VkImageView get_image_view() const { return m_buffers[0].m_image_view; }
    inline VkDeviceMemory get_image_memory() const { return m_buffers[0].m_image_memory; }
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH */
