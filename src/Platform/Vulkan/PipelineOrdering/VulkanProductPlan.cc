#include "VulkanProductPlan.hh"

#include "Platform/Vulkan/Work/VulkanWorkOrchestrator.hh"

namespace esp
{
  VulkanProductPlan::VulkanProductPlan() {}

  VulkanProductPlan::~VulkanProductPlan() {}

  void VulkanProductPlan::add_block(std::shared_ptr<EspBlock> block)
  {
    m_blocks.push_back(std::static_pointer_cast<VulkanBlock>(block));
  }

  void VulkanProductPlan::add_depth_block(std::shared_ptr<EspDepthBlock> depth_block)
  {
    m_depth_block = std::static_pointer_cast<VulkanDepthBlock>(depth_block);
  }

  void VulkanProductPlan::begin_plan()
  {
    // TODO: these structures shouldn't be here. There should be created and init in constructor.
    // TODO: add recreate_because_resize_windows() fuction to recreate these structures (this should be done after
    // the 1. TODO).

    // transition color and depth images for drawing
    {
      // color attachement
      std::vector<VkImageMemoryBarrier> image_memory_barriers = {};

      for (int i = 0; i < m_blocks.size(); i++)
      {
        VkImageMemoryBarrier image_memory_barrier = {};
        image_memory_barrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.dstAccessMask        = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        image_memory_barrier.oldLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barrier.newLayout            = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        image_memory_barrier.image                = m_blocks[i]->get_image();
        image_memory_barrier.subresourceRange     = {
              .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel   = 0,
              .levelCount     = 1,
              .baseArrayLayer = 0,
              .layerCount     = 1,
        };
        image_memory_barriers.push_back(image_memory_barrier);

        if (m_blocks[i]->is_resolvable()) { image_memory_barrier.image = m_blocks[i]->get_resolve_image(); }
        image_memory_barriers.push_back(image_memory_barrier);
      }

      vkCmdPipelineBarrier(VulkanWorkOrchestrator::get_current_command_buffer(),
                           VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,             // srcStageMask
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           static_cast<uint32_t>(image_memory_barriers.size()), // imageMemoryBarrierCount
                           image_memory_barriers.data()                         // pImageMemoryBarriers
      );

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

    std::vector<VkRenderingAttachmentInfoKHR> color_attachment_infos = {};
    color_attachment_infos.resize(m_blocks.size());
    for (int i = 0; i < m_blocks.size(); i++)
    {
      color_attachment_infos[i].sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
      color_attachment_infos[i].imageView   = m_blocks[i]->get_image_view();
      color_attachment_infos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      color_attachment_infos[i].loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
      color_attachment_infos[i].storeOp     = VK_ATTACHMENT_STORE_OP_STORE;

      if (m_blocks[i]->is_resolvable())
      {
        color_attachment_infos[i].resolveImageView   = m_blocks[i]->get_resolve_image_view();
        color_attachment_infos[i].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment_infos[i].resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT;
      }

      auto color                                 = m_blocks[i]->get_clear_color();
      color_attachment_infos[i].clearValue.color = { color.x, color.y, color.z, 1.0f };
    }

    auto [width, height]                = VulkanWorkOrchestrator::get_swap_chain_extent();
    VkRenderingInfoKHR rendering_info   = {};
    rendering_info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea           = { 0, 0, width, height };
    rendering_info.layerCount           = 1;
    rendering_info.colorAttachmentCount = color_attachment_infos.size();
    rendering_info.pColorAttachments    = color_attachment_infos.data();

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

  void VulkanProductPlan::end_plan()
  {
    // end recording commands (vkCmd).....

    VulkanWorkOrchestrator::end_rendering();

    // transition color image to presentation

    std::vector<VkImageMemoryBarrier> image_memory_barriers = {};
    image_memory_barriers.resize(m_blocks.size());

    for (int i = 0; i < m_blocks.size(); i++)
    {
      image_memory_barriers[i].sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      image_memory_barriers[i].srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      image_memory_barriers[i].oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      image_memory_barriers[i].newLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // this may be wrong
      image_memory_barriers[i].subresourceRange = {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
      };

      image_memory_barriers[i].image =
          m_blocks[i]->is_resolvable() ? m_blocks[i]->get_resolve_image() : m_blocks[i]->get_image();
    }

    vkCmdPipelineBarrier(VulkanWorkOrchestrator::get_current_command_buffer(),
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // dstStageMask
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         static_cast<uint32_t>(image_memory_barriers.size()), // imageMemoryBarrierCount
                         image_memory_barriers.data()                         // pImageMemoryBarriers
    );
  }
} // namespace esp
