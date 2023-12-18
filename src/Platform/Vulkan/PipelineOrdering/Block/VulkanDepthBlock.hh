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
    VkImage m_image;
    VkImageView m_image_view;
    VkDeviceMemory m_image_memory;

    bool m_need_layout_transition;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanDepthBlock(EspDepthBlockFormat format, EspSampleCountFlag sample_count_flag, uint32_t width, uint32_t height);
    virtual ~VulkanDepthBlock();

    VulkanDepthBlock(const VulkanDepthBlock&)            = delete;
    VulkanDepthBlock& operator=(const VulkanDepthBlock&) = delete;

    inline virtual void clear() override { on_transition_need(); }

    inline VkImage get_image() const { return m_image; }
    inline VkImageView get_image_view() const { return m_image_view; }

    inline bool need_transition() const { return m_need_layout_transition; }
    inline void on_transition_need() { m_need_layout_transition = true; }
    inline void off_transition_need() { m_need_layout_transition = false; }
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_DEPHT_BLOCK_HH */
