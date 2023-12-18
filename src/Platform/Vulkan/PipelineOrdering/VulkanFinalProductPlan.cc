#include "VulkanFinalProductPlan.hh"

#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanFinalProductPlan::VulkanFinalProductPlan() {}

  VulkanFinalProductPlan::~VulkanFinalProductPlan() {}

  void VulkanFinalProductPlan::add_depth_block(std::shared_ptr<EspDepthBlock> depth_block)
  {
    m_depth_block = std::static_pointer_cast<VulkanDepthBlock>(depth_block);
  }

  void VulkanFinalProductPlan::begin_plan()
  {
    // transition color and depth images for drawing
    {
      // color attachement
      VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
          VulkanSwapChain::get_current_image(),
          0,
          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
          VK_IMAGE_LAYOUT_UNDEFINED,
          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
          VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
          VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
          {
              .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel   = 0,
              .levelCount     = 1,
              .baseArrayLayer = 0,
              .layerCount     = 1,
          });

      // depth attachement
      if (m_depth_block && m_depth_block->need_transition())
      {
        VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
            m_depth_block->get_image(),
            0,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });

        m_depth_block->off_transition_need();
      }
    }

    VkRenderingAttachmentInfoKHR color_attachment_info = {};
    color_attachment_info.sType                        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    color_attachment_info.imageView                    = VulkanSwapChain::get_current_image_view();
    color_attachment_info.imageLayout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_attachment_info.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info.clearValue.color             = { 0.0f, 5.0f, 5.0f, 0.0f };

    auto [width, height]                = VulkanWorkOrchestrator::get_swap_chain_extent();
    VkRenderingInfoKHR rendering_info   = {};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea           = { 0, 0, width, height };
    rendering_info.layerCount           = 1;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments    = &color_attachment_info;

    if (m_depth_block)
    {
      VkRenderingAttachmentInfoKHR depth_stencil_attachment_info{};
      depth_stencil_attachment_info.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      depth_stencil_attachment_info.imageView               = m_depth_block->get_image_view();
      depth_stencil_attachment_info.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      depth_stencil_attachment_info.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
      depth_stencil_attachment_info.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
      depth_stencil_attachment_info.clearValue.depthStencil = { 1.0f, 0 };

      rendering_info.pDepthAttachment   = &depth_stencil_attachment_info;
      rendering_info.pStencilAttachment = VK_NULL_HANDLE;
    }

    VulkanWorkOrchestrator::begin_rendering(&rendering_info);

    // start recording commands (vkCmd).....
  }

  void VulkanFinalProductPlan::end_plan()
  {
    // end recording commands (vkCmd).....

    VulkanWorkOrchestrator::end_rendering();

    // transition color image to presentation
    VulkanWorkOrchestrator::insert_image_memory_barrier_to_current_cmdbuffer(
        VulkanSwapChain::get_current_image(),
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        0,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        });
  }
} // namespace esp
