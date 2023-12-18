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

    std::vector<VulkanBlockBuffer> m_resolve_buffers;

    // TODO: i am not sure if it should be const. Maybe m_buffers.size() should be
    // equal to number of images in swap chain. CHECK IT!
    const uint32_t m_image_index = 0;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanBlock(EspBlockFormat format,
                EspSampleCountFlag sample_count_flag,
                uint32_t width,
                uint32_t height,
                glm::vec3 clear_color);
    virtual ~VulkanBlock();

    VulkanBlock(const VulkanBlock&)            = delete;
    VulkanBlock& operator=(const VulkanBlock&) = delete;

    inline VkImage get_image() const { return m_buffers[m_image_index].m_image; }
    inline VkImageView get_image_view() const { return m_buffers[m_image_index].m_image_view; }
    inline VkDeviceMemory get_image_memory() const { return m_buffers[m_image_index].m_image_memory; }

    inline bool is_resolvable() const { return m_sample_count_flag != EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT; }
    inline VkImage get_resolve_image() const
    {
      ESP_ASSERT(m_resolve_buffers.size() != 0, "The resolve image doesn't exist!");
      return m_resolve_buffers[m_image_index].m_image;
    }
    inline VkImageView get_resolve_image_view() const
    {
      ESP_ASSERT(m_resolve_buffers.size() != 0, "The resolve image view doesn't exist!");
      return m_resolve_buffers[m_image_index].m_image_view;
    }
    inline VkDeviceMemory get_resolve_image_memory() const
    {
      ESP_ASSERT(m_resolve_buffers.size() != 0, "The resolve image memory doesn't exist!");
      return m_resolve_buffers[m_image_index].m_image_memory;
    }
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH */
