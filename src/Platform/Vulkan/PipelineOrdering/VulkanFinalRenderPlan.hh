#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH

// libs
#include "esppch.hh"

// Render API Vulkan
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanBlock.hh"
#include "Platform/Vulkan/PipelineOrdering/Block/VulkanDepthBlock.hh"

// Render API
#include "Core/RenderAPI/PipelineOrdering/EspRenderPlan.hh"

// std
#include <vector>

namespace esp
{
  class VulkanFinalRenderPlan : public EspRenderPlan
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    struct VulkanColorBuffer
    {
      bool is_enable = false;

      VkImage m_image;
      VkImageView m_image_view;
      VkDeviceMemory m_image_memory;

      void terminate();

    } m_color_buffer;

    uint32_t m_height;
    uint32_t m_width;

    const glm::vec3 m_clear_color;
    std::shared_ptr<VulkanDepthBlock> m_depth_block = nullptr;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanFinalRenderPlan(glm::vec3 clear_color);
    virtual ~VulkanFinalRenderPlan();

    VulkanFinalRenderPlan(const VulkanFinalRenderPlan& other)            = delete;
    VulkanFinalRenderPlan& operator=(const VulkanFinalRenderPlan& other) = delete;

    void enable_resolve_block(EspSampleCountFlag sample_count_flag);

    virtual void add_block(std::shared_ptr<EspBlock> block) override {}
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) override;

    virtual void set_command_buffer(EspCommandBufferId* id) override {}
    virtual void build() override;

    virtual void begin_plan() override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_FINAL_PRODUCT_PLAN_HH */