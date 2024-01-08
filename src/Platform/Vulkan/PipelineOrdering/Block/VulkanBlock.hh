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
    friend class VulkanTexture;

    /* -------------------------- FIELDS ----------------------------------- */
   private:
    struct VulkanBlockBuffer
    {
      VkImage m_image;
      VkImageView m_image_view;
      VkDeviceMemory m_image_memory;

      void terminate();
    };

    bool m_buffer_exist = false;
    VulkanBlockBuffer m_buffer;

    bool m_resolve_buffer_exist = false;
    VulkanBlockBuffer m_resolve_buffer;

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

    virtual std::shared_ptr<EspTexture> use_as_texture() const override;
    std::shared_ptr<EspTexture> extract_texture();

    inline VkImage get_image() const { return m_buffer.m_image; }
    inline VkImageView get_image_view() const { return m_buffer.m_image_view; }
    inline VkDeviceMemory get_image_memory() const { return m_buffer.m_image_memory; }

    inline bool is_resolvable() const { return m_sample_count_flag != EspSampleCountFlag::ESP_SAMPLE_COUNT_1_BIT; }
    inline VkImage get_resolve_image() const
    {
      ESP_ASSERT(m_resolve_buffer_exist == true, "The resolve image doesn't exist!");
      return m_resolve_buffer.m_image;
    }
    inline VkImageView get_resolve_image_view() const
    {
      ESP_ASSERT(m_resolve_buffer_exist == true, "The resolve image view doesn't exist!");
      return m_resolve_buffer.m_image_view;
    }
    inline VkDeviceMemory get_resolve_image_memory() const
    {
      ESP_ASSERT(m_resolve_buffer_exist == true, "The resolve image memory doesn't exist!");
      return m_resolve_buffer.m_image_memory;
    }
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_BLOCK_HH */
