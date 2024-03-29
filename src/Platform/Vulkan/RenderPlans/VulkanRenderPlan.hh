#ifndef PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH
#define PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH

// libs
#include "esppch.hh"

// Render API
#include "Core/RenderAPI/RenderPlans/EspRenderPlan.hh"
#include "Platform/Vulkan/RenderPlans/Block/VulkanDepthBlock.hh"

// Render API Vulkan
#include "Platform/Vulkan/RenderPlans/Block/VulkanBlock.hh"

// std
#include <vector>

namespace esp
{
  class VulkanRenderPlan : public EspRenderPlan
  {
    /* -------------------------- FIELDS ----------------------------------- */
   private:
    std::vector<VkImageMemoryBarrier> m_begin_barriers_infos;
    VkImageMemoryBarrier m_depth_begin_barrier_info;
    VkImageMemoryBarrier m_resolvable_depth_begin_barrier_info; // needed if depth block is multisampled
    VkImageMemoryBarrier m_depth_end_barrier_info;

    std::vector<VkImageMemoryBarrier> m_end_barriers_infos;

    std::vector<VkRenderingAttachmentInfoKHR> m_color_attachment_infos;
    VkRenderingAttachmentInfoKHR m_depth_stencil_attachment_info;
    VkRenderingInfoKHR m_rendering_info;

    uint32_t m_height;
    uint32_t m_width;

    std::vector<VkCommandBuffer> m_out_command_buffers;

    VkPipelineStageFlags m_end_src_stage_mask;
    VkPipelineStageFlags m_end_dst_stage_mask;

    std::vector<std::shared_ptr<VulkanBlock>> m_blocks;
    std::shared_ptr<VulkanDepthBlock> m_depth_block = nullptr;

    /* -------------------------- METHODS ---------------------------------- */
   public:
    VulkanRenderPlan();
    virtual ~VulkanRenderPlan();

    VulkanRenderPlan(const VulkanRenderPlan& other)            = delete;
    VulkanRenderPlan& operator=(const VulkanRenderPlan& other) = delete;

    virtual void add_block(std::shared_ptr<EspBlock> block) override;
    virtual void add_depth_block(std::shared_ptr<EspDepthBlock> depth_block) override;

    virtual void set_command_buffer(EspCommandBufferId* id) override;
    virtual void build() override;

    virtual void begin_plan() override;
    virtual void end_plan() override;
  };
} // namespace esp

#endif /* PLATFORM_VULKAN_RENDER_API_VULKAN_PRODUCT_PLAN_HH */