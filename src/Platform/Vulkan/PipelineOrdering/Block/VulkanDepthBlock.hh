#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_DEPHT_BLOCK_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_DEPHT_BLOCK_HH

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
    struct VulkanDepthBlockBuffer
    {
      VkImage m_image;
      VkImageView m_image_view;
      VkDeviceMemory m_image_memory;

      void terminate();
    };

    bool m_need_layout_transition;
    VulkanDepthBlockBuffer m_buffer;

    bool m_resolve_buffer_exist = false;
    VulkanDepthBlockBuffer m_resolve_buffer;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanDepthBlock(EspDepthBlockFormat format,
                     EspSampleCountFlag sample_count_flag,
                     EspImageUsageFlag image_usage_flag,
                     uint32_t width,
                     uint32_t height);
    virtual ~VulkanDepthBlock();

    VulkanDepthBlock(const VulkanDepthBlock&)            = delete;
    VulkanDepthBlock& operator=(const VulkanDepthBlock&) = delete;

    inline virtual void clear() override { on_transition_need(); }

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

    inline bool need_transition() const { return m_need_layout_transition; }
    inline void on_transition_need() { m_need_layout_transition = true; }
    inline void off_transition_need() { m_need_layout_transition = false; }
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_DEPHT_BLOCK_HH */
