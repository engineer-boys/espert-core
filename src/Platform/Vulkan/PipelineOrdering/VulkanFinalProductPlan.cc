#include "VulkanFinalProductPlan.hh"

#include "Platform/Vulkan/Work/VulkanSwapChain.hh"
#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanFinalProductPlan::VulkanFinalProductPlan() {}

  VulkanFinalProductPlan::~VulkanFinalProductPlan() {}

  void VulkanFinalProductPlan::begin_plan()
  {
    // transition color and depth images for drawing
    {
      // color attachement
      const VkImageMemoryBarrier image_memory_barrier{ .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                       .dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                       .oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED,
                                                       .newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                       .image            = VulkanSwapChain::get_current_image(),
                                                       .subresourceRange = {
                                                           .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                                           .baseMipLevel   = 0,
                                                           .levelCount     = 1,
                                                           .baseArrayLayer = 0,
                                                           .layerCount     = 1,
                                                       } };

      vkCmdPipelineBarrier(VulkanWorkOrchestrator::get_current_command_buffer(),
                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,             // srcStageMask
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,                    // imageMemoryBarrierCount
                           &image_memory_barrier // pImageMemoryBarriers
      );

      // depth attachement
      // ...
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
    // rendering_info.pDepthAttachment     = &depthStencilAttachment;
    // rendering_info.pStencilAttachment   = &depthStencilAttachment;

    // this->vkCmdbeginRenderingKHR(m_command_buffers[current_frame], &rendering_info);
    VulkanWorkOrchestrator::begin_rendering(&rendering_info);

    // start recording commands (vkCmd).....
  }

  void VulkanFinalProductPlan::end_plan()
  {
    // end recording commands (vkCmd).....

    VulkanWorkOrchestrator::end_rendering();

    // transition color image to presentation

    const VkImageMemoryBarrier image_memory_barrier{ .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                     .srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                     .oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                     .newLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                     .image            = VulkanSwapChain::get_current_image(),
                                                     .subresourceRange = {
                                                         .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                                         .baseMipLevel   = 0,
                                                         .levelCount     = 1,
                                                         .baseArrayLayer = 0,
                                                         .layerCount     = 1,
                                                     } };

    vkCmdPipelineBarrier(VulkanWorkOrchestrator::get_current_command_buffer(),
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // dstStageMask
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,                    // imageMemoryBarrierCount
                         &image_memory_barrier // pImageMemoryBarriers
    );
  }
} // namespace esp
