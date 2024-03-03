#include "VulkanGuiRenderPlan.hh"
#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  void VulkanGuiRenderPlan::build()
  {
    auto [width, height] = VulkanWorkOrchestrator::get_swap_chain_extent();
    m_width              = width;
    m_height             = height;
  }

  void VulkanGuiRenderPlan::begin_plan(EspRenderArea render_area)
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
    }

    VkRenderingAttachmentInfoKHR color_attachment_info = {};
    color_attachment_info.sType                        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    color_attachment_info.imageView                    = VulkanSwapChain::get_current_image_view();
    color_attachment_info.imageLayout                  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_attachment_info.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info.clearValue.color             = { 0.0f, 0.0f, 0.0f, 0.0f };

    VkRenderingInfoKHR rendering_info   = {};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea           = { render_area.get_render_x(),
                                            render_area.get_render_y(),
                                            render_area.get_render_width(m_width),
                                            render_area.get_render_height(m_height) };
    rendering_info.layerCount           = 1;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments    = &color_attachment_info;

    VulkanWorkOrchestrator::begin_rendering(&rendering_info);

    // start recording commands (vkCmd).....
  }

  void VulkanGuiRenderPlan::end_plan()
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